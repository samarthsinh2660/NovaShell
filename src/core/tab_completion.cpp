#include "core/tab_completion.h"
#include "core/command_registry.h"
#include "git/git_manager.h"
#include "database/db_manager.h"
#include <algorithm>
#include <mutex>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace customos {
namespace core {

struct TabCompletion::Impl {
    std::vector<std::shared_ptr<ICompletionProvider>> providers;
    bool case_sensitive = false;
    bool fuzzy_matching = false;
    int max_suggestions = 20;
    bool smart_completion = true;
    std::vector<std::string> history;
    std::mutex mutex;

    CompletionContext parse_context(const std::string& line, int cursor_pos) {
        CompletionContext ctx;
        ctx.line = line;
        ctx.cursor_position = cursor_pos;
        ctx.current_directory = fs::current_path().string();
        
        // Split into words
        std::istringstream iss(line.substr(0, cursor_pos));
        std::string word;
        while (iss >> word) {
            ctx.words.push_back(word);
        }
        
        if (!ctx.words.empty()) {
            ctx.partial_word = ctx.words.back();
            ctx.word_index = ctx.words.size() - 1;
        }
        
        return ctx;
    }

    bool matches(const std::string& text, const std::string& prefix) {
        if (!case_sensitive) {
            std::string text_lower = text;
            std::string prefix_lower = prefix;
            std::transform(text_lower.begin(), text_lower.end(), text_lower.begin(), ::tolower);
            std::transform(prefix_lower.begin(), prefix_lower.end(), prefix_lower.begin(), ::tolower);
            return text_lower.find(prefix_lower) == 0;
        }
        return text.find(prefix) == 0;
    }

    std::vector<CompletionMatch> filter_matches(const std::vector<CompletionMatch>& matches) {
        std::vector<CompletionMatch> filtered = matches;
        
        // Sort by priority and alphabetically
        std::sort(filtered.begin(), filtered.end(), [](const CompletionMatch& a, const CompletionMatch& b) {
            if (a.priority != b.priority) return a.priority > b.priority;
            return a.text < b.text;
        });
        
        // Limit to max_suggestions
        if (filtered.size() > static_cast<size_t>(max_suggestions)) {
            filtered.resize(max_suggestions);
        }
        
        return filtered;
    }
};

TabCompletion::TabCompletion() : pimpl_(std::make_unique<Impl>()) {
}

TabCompletion::~TabCompletion() = default;

TabCompletion& TabCompletion::instance() {
    static TabCompletion instance;
    return instance;
}

bool TabCompletion::initialize() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    // Register built-in providers
    pimpl_->providers.push_back(std::make_shared<CommandCompletionProvider>());
    pimpl_->providers.push_back(std::make_shared<FileCompletionProvider>());
    pimpl_->providers.push_back(std::make_shared<GitCompletionProvider>());
    pimpl_->providers.push_back(std::make_shared<DatabaseCompletionProvider>());
    
    return true;
}

std::vector<CompletionMatch> TabCompletion::complete(const std::string& line, int cursor_position) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    CompletionContext ctx = pimpl_->parse_context(line, cursor_position);
    std::vector<CompletionMatch> all_matches;
    
    // Get completions from all providers
    for (const auto& provider : pimpl_->providers) {
        auto matches = provider->get_completions(ctx);
        all_matches.insert(all_matches.end(), matches.begin(), matches.end());
    }
    
    return pimpl_->filter_matches(all_matches);
}

std::string TabCompletion::complete_single(const std::string& line, int cursor_position) {
    auto matches = complete(line, cursor_position);
    if (!matches.empty()) {
        return matches[0].text;
    }
    return "";
}

bool TabCompletion::register_provider(std::shared_ptr<ICompletionProvider> provider) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->providers.push_back(provider);
    return true;
}

bool TabCompletion::unregister_provider(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = std::remove_if(pimpl_->providers.begin(), pimpl_->providers.end(),
        [&name](const auto& provider) { return provider->get_name() == name; });
    
    if (it != pimpl_->providers.end()) {
        pimpl_->providers.erase(it, pimpl_->providers.end());
        return true;
    }
    return false;
}

std::vector<CompletionMatch> TabCompletion::complete_commands(const std::string& prefix) {
    // TODO: Get commands from registry
    std::vector<CompletionMatch> matches;
    
    std::vector<std::string> commands = {
        "help", "version", "exit", "git", "db", "vault-add", "net-sniff", 
        "monitor-start", "ai-suggest", "note-add", "env-switch"
    };
    
    for (const auto& cmd : commands) {
        if (pimpl_->matches(cmd, prefix)) {
            CompletionMatch match;
            match.text = cmd;
            match.description = "Command";
            match.priority = 10;
            matches.push_back(match);
        }
    }
    
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_files(const std::string& prefix) {
    std::vector<CompletionMatch> matches;
    
    try {
        fs::path search_path = prefix.empty() ? "." : prefix;
        fs::path dir = search_path.parent_path();
        if (dir.empty()) dir = ".";
        
        for (const auto& entry : fs::directory_iterator(dir)) {
            std::string filename = entry.path().filename().string();
            if (pimpl_->matches(filename, prefix)) {
                CompletionMatch match;
                match.text = entry.path().string();
                match.description = entry.is_directory() ? "Directory" : "File";
                match.priority = entry.is_directory() ? 8 : 5;
                matches.push_back(match);
            }
        }
    } catch (...) {
        // Ignore errors
    }
    
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_directories(const std::string& prefix) {
    std::vector<CompletionMatch> matches;
    
    try {
        fs::path search_path = prefix.empty() ? "." : prefix;
        fs::path dir = search_path.parent_path();
        if (dir.empty()) dir = ".";
        
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_directory()) {
                std::string dirname = entry.path().filename().string();
                if (pimpl_->matches(dirname, prefix)) {
                    CompletionMatch match;
                    match.text = entry.path().string();
                    match.description = "Directory";
                    match.priority = 8;
                    matches.push_back(match);
                }
            }
        }
    } catch (...) {
        // Ignore errors
    }
    
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_users(const std::string& prefix) {
    // TODO: Get users from authentication system
    (void)prefix; // Suppress unused parameter warning
    return {};
}

std::vector<CompletionMatch> TabCompletion::complete_environment_vars(const std::string& prefix) {
    // TODO: Get environment variables
    (void)prefix; // Suppress unused parameter warning
    return {};
}

std::vector<CompletionMatch> TabCompletion::complete_git_branches(const std::string& prefix) {
    std::vector<CompletionMatch> matches;
    
    auto branches = git::GitManager::instance().list_branches(false);
    for (const auto& branch : branches) {
        if (pimpl_->matches(branch.name, prefix)) {
            CompletionMatch match;
            match.text = branch.name;
            match.description = branch.is_current ? "Current branch" : "Branch";
            match.priority = branch.is_current ? 10 : 7;
            matches.push_back(match);
        }
    }
    
    (void)prefix; // Suppress unused parameter warning
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_db_tables(const std::string& prefix) {
    std::vector<CompletionMatch> matches;
    
#ifdef HAVE_SQLITE3
    auto tables = database::DBManager::instance().list_tables();
    for (const auto& table : tables) {
        if (pimpl_->matches(table, prefix)) {
            CompletionMatch match;
            match.text = table;
            match.description = "Table";
            match.priority = 7;
            matches.push_back(match);
        }
    }
#endif
    
    (void)prefix; // Suppress unused parameter warning
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_db_columns(const std::string& table, const std::string& prefix) {
    std::vector<CompletionMatch> matches;
    
#ifdef HAVE_SQLITE3
    auto columns = database::DBManager::instance().list_columns(table);
    for (const auto& column : columns) {
        if (pimpl_->matches(column, prefix)) {
            CompletionMatch match;
            match.text = column;
            match.description = "Column";
            match.priority = 7;
            matches.push_back(match);
        }
    }
#endif
    
    (void)table;   // Suppress unused parameter warning
    (void)prefix;  // Suppress unused parameter warning
    return matches;
}

std::vector<CompletionMatch> TabCompletion::complete_plugins(const std::string& prefix) {
    (void)prefix; // Suppress unused parameter warning
    // TODO: Get plugins from plugin manager
    return {};
}

void TabCompletion::set_case_sensitive(bool sensitive) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->case_sensitive = sensitive;
}

void TabCompletion::set_fuzzy_matching(bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->fuzzy_matching = enabled;
}

void TabCompletion::set_max_suggestions(int max) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->max_suggestions = max;
}

void TabCompletion::add_to_history(const std::string& command) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->history.push_back(command);
    if (pimpl_->history.size() > 1000) {
        pimpl_->history.erase(pimpl_->history.begin());
    }
}

std::vector<CompletionMatch> TabCompletion::complete_from_history(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<CompletionMatch> matches;
    for (const auto& cmd : pimpl_->history) {
        if (pimpl_->matches(cmd, prefix)) {
            CompletionMatch match;
            match.text = cmd;
            match.description = "From history";
            match.priority = 6;
            matches.push_back(match);
        }
    }
    
    return matches;
}

void TabCompletion::enable_smart_completion(bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->smart_completion = enabled;
}

std::vector<CompletionMatch> TabCompletion::get_smart_suggestions(const CompletionContext& context) {
    // TODO: Implement smart context-aware suggestions
    return {};
}

// Built-in providers implementation

std::vector<CompletionMatch> CommandCompletionProvider::get_completions(const CompletionContext& context) {
    if (context.word_index == 0) {
        return TabCompletion::instance().complete_commands(context.partial_word);
    }
    return {};
}

std::vector<CompletionMatch> FileCompletionProvider::get_completions(const CompletionContext& context) {
    if (context.word_index > 0) {
        return TabCompletion::instance().complete_files(context.partial_word);
    }
    return {};
}

std::vector<CompletionMatch> GitCompletionProvider::get_completions(const CompletionContext& context) {
    if (context.words.empty()) return {};
    
    if (context.words[0] == "git") {
        if (context.word_index == 1) {
            // Git subcommands
            std::vector<std::string> subcommands = {
                "status", "add", "commit", "push", "pull", "branch", "checkout", "merge", "log"
            };
            
            std::vector<CompletionMatch> matches;
            for (const auto& cmd : subcommands) {
                if (cmd.find(context.partial_word) == 0) {
                    CompletionMatch match;
                    match.text = cmd;
                    match.description = "Git command";
                    match.priority = 9;
                    matches.push_back(match);
                }
            }
            return matches;
        }
        else if (context.words.size() > 1 && (context.words[1] == "checkout" || context.words[1] == "branch")) {
            // Complete branch names
            return TabCompletion::instance().complete_git_branches(context.partial_word);
        }
    }
    return {};
}

std::vector<CompletionMatch> DatabaseCompletionProvider::get_completions(const CompletionContext& context) {
    if (context.words.empty()) return {};
    
    if (context.words[0] == "db") {
        if (context.word_index == 1) {
            // DB subcommands
            std::vector<std::string> subcommands = {
                "connect", "query", "list-tables", "list-databases", "export", "import"
            };
            
            std::vector<CompletionMatch> matches;
            for (const auto& cmd : subcommands) {
                if (cmd.find(context.partial_word) == 0) {
                    CompletionMatch match;
                    match.text = cmd;
                    match.description = "DB command";
                    match.priority = 9;
                    matches.push_back(match);
                }
            }
            return matches;
        }
    }
    return {};
}

} // namespace core
} // namespace customos
