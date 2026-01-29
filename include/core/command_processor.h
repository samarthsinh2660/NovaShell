#ifndef CUSTOMOS_COMMAND_PROCESSOR_H
#define CUSTOMOS_COMMAND_PROCESSOR_H

#include <string>
#include <vector>
#include <memory>
#include "command_registry.h"

namespace customos {
namespace core {

struct CommandResult {
    bool success;
    std::string output;
    int exit_code;
};

class CommandProcessor {
public:
    CommandProcessor();
    ~CommandProcessor();

    // Initialize processor and register built-in commands
    bool initialize();

    // Process and execute a command line
    CommandResult process(const std::string& command_line);

    // Get command registry
    CommandRegistry* get_registry();

private:
    struct ParsedCommand {
        std::string name;
        std::vector<std::string> arguments;
        std::string input_redirect;
        std::string output_redirect;
        bool append_output;
        bool background;
    };

    ParsedCommand parse_command(const std::string& command_line);
    CommandResult execute_parsed_command(const ParsedCommand& cmd);
    void register_builtin_commands();
    void register_scheduler_commands();
    void register_ai_commands();
    void register_plugin_commands();

    // Helper function for category help display
    static void show_category_help(const std::string& category_name, const std::vector<std::pair<std::string, std::string>>& commands);

    std::unique_ptr<CommandRegistry> registry_;
};

} // namespace core
} // namespace customos

#endif // CUSTOMOS_COMMAND_PROCESSOR_H
