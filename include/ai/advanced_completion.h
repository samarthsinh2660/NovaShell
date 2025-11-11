#ifndef NOVASHELL_ADVANCED_COMPLETION_H
#define NOVASHELL_ADVANCED_COMPLETION_H

#include <string>
#include <vector>
#include <memory>

namespace novashell {
namespace ai {

// Enhanced completion match with AI scoring
struct AdvancedCompletionMatch {
    std::string text;
    std::string description;
    float ai_score;        // AI confidence (0.0-1.0)
    float fuzzy_score;     // Fuzzy match score
    float frequency_score; // Usage frequency
    float context_score;   // Context relevance
    std::string category;
    std::vector<std::string> aliases;
};

// Fuzzy matching options
struct FuzzyOptions {
    bool case_sensitive = false;
    int max_distance = 3;           // Levenshtein distance
    bool allow_gaps = true;
    bool prefer_start_match = true;
};

// Advanced Tab Completion Enhancement
class AdvancedCompletion {
public:
    static AdvancedCompletion& instance();

    // Initialize with AI backend
    bool initialize(const std::string& gemini_api_key = "");

    // Enhanced fuzzy completion
    std::vector<AdvancedCompletionMatch> fuzzy_complete(
        const std::string& input, 
        const std::vector<std::string>& candidates,
        const FuzzyOptions& options = FuzzyOptions()
    );

    // AI-powered smart completion
    std::vector<AdvancedCompletionMatch> ai_complete(
        const std::string& context,
        const std::string& partial_input
    );

    // Context-aware suggestions
    std::vector<AdvancedCompletionMatch> context_complete(
        const std::string& command_line,
        int cursor_position,
        const std::map<std::string, std::string>& context
    );

    // Learning from usage
    void record_completion_usage(const std::string& completion);
    std::vector<std::string> get_frequent_completions(int limit = 10);
    void clear_usage_history();

    // Fuzzy search algorithms
    int levenshtein_distance(const std::string& s1, const std::string& s2);
    float fuzzy_match_score(const std::string& pattern, const std::string& text);
    bool fuzzy_match(const std::string& pattern, const std::string& text, int max_distance = 3);

    // Smart suggestions based on context
    std::vector<std::string> suggest_next_commands(
        const std::vector<std::string>& recent_commands
    );

    // Abbreviation expansion
    std::string expand_abbreviation(const std::string& abbr);
    bool register_abbreviation(const std::string& abbr, const std::string& expansion);

    // Multi-word completion
    std::vector<AdvancedCompletionMatch> complete_multi_word(
        const std::string& input,
        const std::vector<std::vector<std::string>>& word_sequences
    );

    // Configuration
    void enable_ai_completion(bool enabled);
    void enable_fuzzy_matching(bool enabled);
    void set_max_suggestions(int max);
    void set_min_ai_score(float min_score);

private:
    AdvancedCompletion();
    ~AdvancedCompletion();
    AdvancedCompletion(const AdvancedCompletion&) = delete;
    AdvancedCompletion& operator=(const AdvancedCompletion&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace ai
} // namespace novashell

#endif // NOVASHELL_ADVANCED_COMPLETION_H
