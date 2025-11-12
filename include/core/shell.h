#ifndef CUSTOMOS_SHELL_H
#define CUSTOMOS_SHELL_H

#include <string>
#include <memory>
#include "command_processor.h"
#include "../auth/authentication.h"
#include "../logging/logger.h"

namespace customos {
namespace core {

class Shell {
public:
    Shell();
    ~Shell();

    // Initialize the shell system
    bool initialize();

    // Main execution loop
    void run();

    // Execute a single command
    bool execute_command(const std::string& command);

    // Shutdown the shell
    void shutdown();

    // Get current prompt
    std::string get_prompt() const;

    // Set custom prompt
    void set_prompt(const std::string& prompt);

private:
    void display_welcome();
    void show_help();
    void load_configuration();
    void save_history();
    std::string read_input();
    void handle_signal(int signal);

    std::unique_ptr<CommandProcessor> command_processor_;
    
    std::string prompt_;
    bool running_;
    bool initialized_;
};

} // namespace core
} // namespace customos

#endif // CUSTOMOS_SHELL_H
