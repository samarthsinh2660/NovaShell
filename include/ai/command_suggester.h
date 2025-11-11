#ifndef CUSTOMOS_COMMAND_SUGGESTER_H
#define CUSTOMOS_COMMAND_SUGGESTER_H

#include <string>
#include <vector>
#include <memory>

namespace customos {
namespace ai {

// Command suggestion with confidence score
struct CommandSuggestion {
    std::string command;
    std::string description;
    float confidence;  // 0.0 to 1.0
    std::string category;
};

// Context for AI suggestions
struct SuggestionContext {
    std::string current_directory;
    std::string current_user;
    std::vector<std::string> recent_commands;
    std::string partial_input;
    std::map<std::string, std::string> environment_vars;
};

// AI-Powered Command Suggester (using Gemini API)
class CommandSuggester {
public:
    static CommandSuggester& instance();

    // Initialize with API key
    bool initialize(const std::string& api_key);
    bool is_initialized() const;

    // Get command suggestions based on context
    std::vector<CommandSuggestion> suggest(const SuggestionContext& context);
    
    // Get suggestions for partial command
    std::vector<CommandSuggestion> autocomplete(const std::string& partial);

    // Learn from command execution (improve suggestions)
    void learn_from_execution(const std::string& command, bool success);

    // Get suggestion for next likely command
    std::vector<CommandSuggestion> predict_next_command();

    // Enable/disable AI suggestions
    void enable(bool enabled);
    bool is_enabled() const;

    // Set API endpoint
    void set_api_endpoint(const std::string& endpoint);

private:
    CommandSuggester();
    ~CommandSuggester();
    CommandSuggester(const CommandSuggester&) = delete;
    CommandSuggester& operator=(const CommandSuggester&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace ai
} // namespace customos

#endif // CUSTOMOS_COMMAND_SUGGESTER_H
