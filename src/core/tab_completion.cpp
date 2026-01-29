#include "core/tab_completion.h"
#include "core/command_registry.h"
#include "git/git_manager.h"
#include "database/db_manager.h"
#include "ai/ai_module.h"
#include "ai/ai_prompt_manager.h"
#include "auth/authentication.h"
#include "database/internal_db.h"
#include <algorithm>
#include <mutex>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <iomanip>

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

    // Learning system
    struct CommandPattern {
        std::string command;
        std::string context;  // time, day, location
        int frequency = 1;
        std::chrono::system_clock::time_point last_used;
        std::unordered_set<std::string> next_commands;  // commands used after this one
        std::unordered_map<std::string, int> argument_patterns;  // common arguments
    };

    std::unordered_map<std::string, CommandPattern> command_patterns;
    std::unordered_map<std::string, std::vector<std::string>> workflow_sequences;
    std::unordered_map<std::string, int> time_based_patterns;  // hour -> command frequency
    std::unordered_map<std::string, int> context_patterns;     // context -> command frequency

    // AI suggestion cache
    struct AISuggestion {
        std::string input;
        std::vector<std::string> suggestions;
        std::chrono::system_clock::time_point timestamp;
    };
    std::unordered_map<std::string, AISuggestion> ai_cache;

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
    pimpl_->providers.push_back(std::make_shared<AICompletionProvider>());  // AI suggestions
    pimpl_->providers.push_back(std::make_shared<LearningCompletionProvider>());  // Learning-based

    // Load learning data
    load_learning_data();
    
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
        "help", "version", "exit", "quit", "clear", "cls", "whoami",
        "login", "logout", "create-user", "adduser",
        "vault-init", "vault-unlock", "vault-lock", "vault-add", "vault-get", "vault-list", "vault-delete", "vault-search",
        "git", "git-status", "git-add", "git-commit", "git-push", "git-pull", "git-branch", "git-checkout", "git-log",
        "db", "db-connect", "db-query", "db-list-tables", "db-export", "db-import",
        "net-interfaces", "net-stats", "net-capture", "net-stop", "net-packets", "net-protocols",
        "monitor-start", "monitor-stats", "proc-list", "proc-kill",
        "note-add", "note-list", "note-get", "snippet-add", "snippet-get", "snippet-search",
        "env-create", "env-switch", "env-list",
        "task-schedule", "task-list", "task-cancel",
        "remind", "remind-list", "remind-dismiss",
        "p2p-start", "p2p-share", "p2p-list", "p2p-unshare",
        "ai-init", "ai-suggest", "ai-completion-stats",
        "theme-list", "theme-set", "theme-create",
        "ssh-start", "ssh-stop", "ssh-connections",
        "container-list", "container-start", "container-stop", "image-pull", "image-build",
        "docker-init", "docker-ps",
        "mobile-api-start", "mobile-api-stop", "mobile-api-status",
        "remote-desktop-start", "remote-desktop-sessions",
        "remote-desktop-terminals", "remote-desktop-switch", "remote-desktop-capture-terminal",
        "remote-desktop-displays", "remote-desktop-fullscreen", "remote-desktop-set-display",
        "remote-desktop-windows",
        "analytics-dashboard", "analytics-insights", "analytics-metrics"
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
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    std::vector<CompletionMatch> suggestions;

    // Get AI suggestions
    auto ai_suggestions = get_ai_suggestions(context);
    suggestions.insert(suggestions.end(), ai_suggestions.begin(), ai_suggestions.end());

    // Get learning-based suggestions
    auto learned_suggestions = get_learned_suggestions(context);
    suggestions.insert(suggestions.end(), learned_suggestions.begin(), learned_suggestions.end());

    // Get context-aware suggestions
    auto context_suggestions = get_context_suggestions(context);
    suggestions.insert(suggestions.end(), context_suggestions.begin(), context_suggestions.end());

    return suggestions;
}

std::vector<CompletionMatch> TabCompletion::get_ai_suggestions(const CompletionContext& context) {
    std::vector<CompletionMatch> suggestions;

    if (!ai::APIKeyManager::instance().has_api_key()) {
        return suggestions;
    }

    // Create cache key from context
    std::string cache_key = context.line.substr(0, context.cursor_position);
    cache_key += "|" + context.current_directory;

    // Check cache first (5 minute expiry)
    auto now = std::chrono::system_clock::now();
    if (pimpl_->ai_cache.count(cache_key)) {
        auto& cached = pimpl_->ai_cache[cache_key];
        auto age = now - cached.timestamp;
        if (std::chrono::duration_cast<std::chrono::minutes>(age).count() < 5) {
            for (const auto& suggestion : cached.suggestions) {
                CompletionMatch match;
                match.text = suggestion;
                match.description = "🤖 AI suggestion";
                match.priority = 15;  // High priority for AI
                suggestions.push_back(match);
            }
            return suggestions;
        }
    }

    // Get AI suggestions from Gemini
    try {
        // Build comprehensive context for AI
        std::string user_context = auth::Authentication::instance().get_current_user();
        std::string current_time = "";
        std::string recent_commands = "";

        // Get current time context
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_now = *std::localtime(&time_t_now);
        char time_buffer[20];
        std::strftime(time_buffer, sizeof(time_buffer), "%H:%M", &tm_now);
        current_time = time_buffer;

        // Get recent command history (last 5 commands)
        try {
            auto& db = database::InternalDB::instance();
            auto history = db.get_history(5);
            if (!history.empty()) {
                recent_commands = "Recent commands used:\n";
                for (size_t i = 0; i < std::min(size_t(3), history.size()); ++i) {
                    recent_commands += "- " + history[i] + "\n";
                }
            }
        } catch (...) {
            // Ignore history loading errors
        }

        // Build context for prompt manager
        ai::PromptContext prompt_context;
        prompt_context.task_description = context.line.substr(0, context.cursor_position);
        prompt_context.current_directory = context.current_directory;
        prompt_context.user = user_context;
        prompt_context.context_info = "Current time: " + current_time + "\n" + recent_commands;

        // Generate optimized prompt using AIPromptManager
        std::string prompt = ai::AIPromptManager::instance().generate_tab_completion_prompt(prompt_context);

        auto response = ai::GeminiClient::instance().generate_content(prompt);

        if (response.success && !response.content.empty()) {
            std::istringstream iss(response.content);
            std::string line;
            while (std::getline(iss, line) && suggestions.size() < 5) {
                // Clean up the line
                line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) { return !std::isspace(ch); }));
                line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) { return !std::isspace(ch); }).base(), line.end());

                // Skip empty lines and lines that start with common response prefixes
                if (!line.empty() && line.find("Here are") != 0 && line.find("Based on") != 0
                    && line.find("- ") != 0 && line.find("* ") != 0 && line.find("!= ") != 0) {

                    CompletionMatch match;
                    match.text = line;
                    match.description = "🤖 AI suggestion";
                    match.priority = 15;  // High priority for AI
                    suggestions.push_back(match);

                    // Cache the suggestion
                    pimpl_->ai_cache[cache_key].suggestions.push_back(line);
                }
            }

            if (!suggestions.empty()) {
                pimpl_->ai_cache[cache_key].timestamp = now;
            }
        }
    } catch (...) {
        // Ignore AI errors, just return empty suggestions
    }

    return suggestions;
}

std::vector<CompletionMatch> TabCompletion::get_learned_suggestions(const CompletionContext& context) {
    std::vector<CompletionMatch> suggestions;

    if (context.words.empty()) return suggestions;

    std::string current_cmd = context.words[0];
    std::string partial = context.partial_word;

    // Find patterns for current command
    if (pimpl_->command_patterns.count(current_cmd)) {
        const auto& pattern = pimpl_->command_patterns[current_cmd];

        // Suggest common arguments
        for (const auto& arg_pair : pattern.argument_patterns) {
            if (arg_pair.first.find(partial) != std::string::npos && arg_pair.second > 2) {
                CompletionMatch match;
                match.text = arg_pair.first;
                match.description = "📊 Learned argument (" + std::to_string(arg_pair.second) + " uses)";
                match.priority = 12;
                suggestions.push_back(match);
            }
        }
    }

    // Suggest next commands in workflow
    if (pimpl_->workflow_sequences.count(current_cmd)) {
        for (const auto& next_cmd : pimpl_->workflow_sequences[current_cmd]) {
            if (next_cmd.find(partial) == 0) {
                CompletionMatch match;
                match.text = next_cmd;
                match.description = "🔄 Workflow suggestion";
                match.priority = 11;
                suggestions.push_back(match);
            }
        }
    }

    return suggestions;
}

std::vector<CompletionMatch> TabCompletion::get_context_suggestions(const CompletionContext& context) {
    std::vector<CompletionMatch> suggestions;

    // Get current time context
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time_t);
    int hour = tm.tm_hour;
    std::string time_context = std::to_string(hour);

    // Get day context
    std::string day_context = (tm.tm_wday == 0 || tm.tm_wday == 6) ? "weekend" : "weekday";

    // Find time-based patterns
    if (pimpl_->time_based_patterns.count(time_context)) {
        // Suggest commands commonly used at this time
        for (const auto& pattern : pimpl_->command_patterns) {
            if (pattern.second.frequency > 3 && context.partial_word.empty()) {
                CompletionMatch match;
                match.text = pattern.first;
                match.description = "⏰ Time-based suggestion";
                match.priority = 10;
                suggestions.push_back(match);
            }
        }
    }

    return suggestions;
}

void TabCompletion::learn_from_command(const std::string& command, const std::string& previous_command) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (command.empty()) return;

    auto now = std::chrono::system_clock::now();

    // Parse command
    std::istringstream iss(command);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    if (words.empty()) return;

    std::string cmd = words[0];

    // Update command pattern
    if (!pimpl_->command_patterns.count(cmd)) {
        pimpl_->command_patterns[cmd] = Impl::CommandPattern{cmd, "", 0, now};
    }

    auto& pattern = pimpl_->command_patterns[cmd];
    pattern.frequency++;
    pattern.last_used = now;

    // Learn arguments
    if (words.size() > 1) {
        std::string args = command.substr(cmd.length());
        args.erase(args.begin(), std::find_if(args.begin(), args.end(), [](int ch) { return !std::isspace(ch); }));
        if (!args.empty()) {
            pattern.argument_patterns[args]++;
        }
    }

    // Learn workflow sequences
    if (!previous_command.empty()) {
        std::istringstream prev_iss(previous_command);
        std::string prev_cmd;
        prev_iss >> prev_cmd;

        if (!prev_cmd.empty()) {
            pimpl_->workflow_sequences[prev_cmd].push_back(cmd);

            // Keep only most common next commands (limit to 10)
            if (pimpl_->workflow_sequences[prev_cmd].size() > 10) {
                // Sort by frequency and keep top 10
                std::unordered_map<std::string, int> freq;
                for (const auto& next : pimpl_->workflow_sequences[prev_cmd]) {
                    freq[next]++;
                }

                std::vector<std::pair<std::string, int>> sorted(freq.begin(), freq.end());
                std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
                    return a.second > b.second;
                });

                pimpl_->workflow_sequences[prev_cmd].clear();
                for (size_t i = 0; i < std::min(size_t(10), sorted.size()); ++i) {
                    pimpl_->workflow_sequences[prev_cmd].push_back(sorted[i].first);
                }
            }
        }
    }

    // Learn time-based patterns
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time_t);
    std::string hour_key = std::to_string(tm.tm_hour);
    pimpl_->time_based_patterns[hour_key]++;

    // Store in database for persistence
    try {
        auto& db = database::InternalDB::instance();
        db.add_analytics_data("command_usage_" + cmd, 1.0, "count",
                             "{\"hour\": " + std::to_string(tm.tm_hour) + ", \"day\": " + std::to_string(tm.tm_wday) + "}");
    } catch (...) {
        // Ignore database errors
    }
}

void TabCompletion::load_learning_data() {
    try {
        auto& db = database::InternalDB::instance();
        auto analytics = db.get_analytics_data("command_usage_", 1000);

        for (const auto& data : analytics) {
            if (data.count("metric_name") && data.at("metric_name").find("command_usage_") == 0) {
                std::string cmd = data.at("metric_name").substr(14); // Remove "command_usage_" prefix
                if (!pimpl_->command_patterns.count(cmd)) {
                    pimpl_->command_patterns[cmd] = Impl::CommandPattern{cmd, "", 0, std::chrono::system_clock::now()};
                }
                if (data.count("value")) {
                    pimpl_->command_patterns[cmd].frequency += static_cast<int>(std::stod(data.at("value")));
                }
            }
        }
    } catch (...) {
        // Ignore loading errors
    }
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

std::vector<CompletionMatch> AICompletionProvider::get_completions(const CompletionContext& context) {
    return TabCompletion::instance().get_ai_suggestions(context);
}

std::vector<CompletionMatch> LearningCompletionProvider::get_completions(const CompletionContext& context) {
    std::vector<CompletionMatch> suggestions;

    // Combine learned and context suggestions
    auto learned = TabCompletion::instance().get_learned_suggestions(context);
    auto context_based = TabCompletion::instance().get_context_suggestions(context);

    suggestions.insert(suggestions.end(), learned.begin(), learned.end());
    suggestions.insert(suggestions.end(), context_based.begin(), context_based.end());

    return suggestions;
}

} // namespace core
} // namespace customos
