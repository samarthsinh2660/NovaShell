#ifndef CUSTOMOS_TAB_COMPLETION_H
#define CUSTOMOS_TAB_COMPLETION_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace customos {
namespace core {

// Completion match
struct CompletionMatch {
    std::string text;
    std::string description;
    std::string display;
    int priority;  // Higher = shown first
};

// Completion context
struct CompletionContext {
    std::string line;
    int cursor_position;
    std::vector<std::string> words;
    int word_index;
    std::string partial_word;
    std::string current_directory;
    std::string current_user;
};

// Completion provider interface
class ICompletionProvider {
public:
    virtual ~ICompletionProvider() = default;
    virtual std::vector<CompletionMatch> get_completions(const CompletionContext& context) = 0;
    virtual std::string get_name() const = 0;
};

// Tab Completion Engine
class TabCompletion {
public:
    static TabCompletion& instance();

    // Initialize completion system
    bool initialize();

    // Main completion function
    std::vector<CompletionMatch> complete(const std::string& line, int cursor_position);

    // Get single best match
    std::string complete_single(const std::string& line, int cursor_position);

    // Register custom completion providers
    bool register_provider(std::shared_ptr<ICompletionProvider> provider);
    bool unregister_provider(const std::string& name);

    // Built-in completers
    std::vector<CompletionMatch> complete_commands(const std::string& prefix);
    std::vector<CompletionMatch> complete_files(const std::string& prefix);
    std::vector<CompletionMatch> complete_directories(const std::string& prefix);
    std::vector<CompletionMatch> complete_users(const std::string& prefix);
    std::vector<CompletionMatch> complete_environment_vars(const std::string& prefix);
    std::vector<CompletionMatch> complete_git_branches(const std::string& prefix);
    std::vector<CompletionMatch> complete_db_tables(const std::string& prefix);
    std::vector<CompletionMatch> complete_db_columns(const std::string& table, const std::string& prefix);
    std::vector<CompletionMatch> complete_plugins(const std::string& prefix);

    // Configuration
    void set_case_sensitive(bool sensitive);
    void set_fuzzy_matching(bool enabled);
    void set_max_suggestions(int max);

    // History-based completion
    void add_to_history(const std::string& command);
    std::vector<CompletionMatch> complete_from_history(const std::string& prefix);

    // Smart completion (context-aware)
    void enable_smart_completion(bool enabled);
    std::vector<CompletionMatch> get_smart_suggestions(const CompletionContext& context);

private:
    TabCompletion();
    ~TabCompletion();
    TabCompletion(const TabCompletion&) = delete;
    TabCompletion& operator=(const TabCompletion&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Built-in completion providers

class CommandCompletionProvider : public ICompletionProvider {
public:
    std::vector<CompletionMatch> get_completions(const CompletionContext& context) override;
    std::string get_name() const override { return "commands"; }
};

class FileCompletionProvider : public ICompletionProvider {
public:
    std::vector<CompletionMatch> get_completions(const CompletionContext& context) override;
    std::string get_name() const override { return "files"; }
};

class GitCompletionProvider : public ICompletionProvider {
public:
    std::vector<CompletionMatch> get_completions(const CompletionContext& context) override;
    std::string get_name() const override { return "git"; }
};

class DatabaseCompletionProvider : public ICompletionProvider {
public:
    std::vector<CompletionMatch> get_completions(const CompletionContext& context) override;
    std::string get_name() const override { return "database"; }
};

} // namespace core
} // namespace customos

#endif // CUSTOMOS_TAB_COMPLETION_H
