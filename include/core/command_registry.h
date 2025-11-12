#ifndef CUSTOMOS_COMMAND_REGISTRY_H
#define CUSTOMOS_COMMAND_REGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <mutex>

namespace customos {
namespace core {

// Command execution context
struct CommandContext {
    std::vector<std::string> args;
    std::string current_user;
    std::string working_directory;
    std::map<std::string, std::string> environment;
};

// Command handler function type
using CommandHandler = std::function<int(const CommandContext&)>;

// Command metadata
struct CommandInfo {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> required_permissions;
    CommandHandler handler;
};

class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();

    // Register a new command
    bool register_command(const CommandInfo& cmd_info);

    // Unregister a command
    bool unregister_command(const std::string& name);

    // Check if command exists
    bool has_command(const std::string& name) const;

    // Get command information
    const CommandInfo* get_command(const std::string& name) const;

    // Execute a command
    int execute(const std::string& name, const CommandContext& context);

    // List all registered commands
    std::vector<std::string> list_commands() const;

    // Get command suggestions (for autocomplete)
    std::vector<std::string> suggest_commands(const std::string& prefix) const;

private:
    std::map<std::string, CommandInfo> commands_;
    mutable std::mutex mutex_;
};

} // namespace core
} // namespace customos

#endif // CUSTOMOS_COMMAND_REGISTRY_H
