#include "core/command_processor.h"
#include "core/command_registry.h"
#include "auth/authentication.h"
#include "vfs/virtual_filesystem.h"
#include "vault/password_manager.h"
#include "network/packet_analyzer.h"
#include "plugins/plugin_manager.h"
#include "scripting/script_engine.h"
#include "logging/logger.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace customos {
namespace core {

CommandProcessor::CommandProcessor() {
    registry_ = std::make_unique<CommandRegistry>();
}

CommandProcessor::~CommandProcessor() = default;

bool CommandProcessor::initialize() {
    register_builtin_commands();
    return true;
}

CommandResult CommandProcessor::process(const std::string& command_line) {
    CommandResult result;
    result.success = false;
    result.exit_code = 1;

    try {
        // Parse the command
        ParsedCommand cmd = parse_command(command_line);

        if (cmd.name.empty()) {
            result.output = "Error: Empty command\n";
            return result;
        }

        // Execute the command
        result = execute_parsed_command(cmd);
    }
    catch (const std::exception& e) {
        result.output = std::string("Command execution error: ") + e.what() + "\n";
        result.success = false;
        result.exit_code = 1;
    }

    return result;
}

CommandRegistry* CommandProcessor::get_registry() {
    return registry_.get();
}

CommandProcessor::ParsedCommand CommandProcessor::parse_command(const std::string& command_line) {
    ParsedCommand cmd;
    cmd.append_output = false;
    cmd.background = false;

    std::istringstream iss(command_line);
    std::string token;

    // Extract command name
    if (!(iss >> cmd.name)) {
        return cmd;
    }

    // Parse arguments and special operators
    while (iss >> token) {
        if (token == ">") {
            iss >> cmd.output_redirect;
            cmd.append_output = false;
        }
        else if (token == ">>") {
            iss >> cmd.output_redirect;
            cmd.append_output = true;
        }
        else if (token == "<") {
            iss >> cmd.input_redirect;
        }
        else if (token == "&") {
            cmd.background = true;
        }
        else {
            cmd.arguments.push_back(token);
        }
    }

    return cmd;
}

CommandResult CommandProcessor::execute_parsed_command(const ParsedCommand& cmd) {
    CommandResult result;

    // Check if command exists
    if (!registry_->has_command(cmd.name)) {
        result.output = "Command not found: " + cmd.name + "\n";
        result.success = false;
        result.exit_code = 127;
        return result;
    }

    // Create command context
    CommandContext context;
    context.args = cmd.arguments;
    context.current_user = auth::Authentication::instance().get_current_user();
    context.working_directory = "/"; // TODO: Implement working directory

    // Execute the command
    result.exit_code = registry_->execute(cmd.name, context);
    result.success = (result.exit_code == 0);

    return result;
}

void CommandProcessor::register_builtin_commands() {
    // Help command
    CommandInfo help_cmd;
    help_cmd.name = "help";
    help_cmd.description = "Display help information";
    help_cmd.usage = "help [command]";
    help_cmd.handler = [this](const CommandContext& ctx) -> int {
        if (ctx.args.empty()) {
            std::cout << "Available commands:\n\n";
            auto commands = registry_->list_commands();
            std::sort(commands.begin(), commands.end());
            for (const auto& cmd_name : commands) {
                const auto* info = registry_->get_command(cmd_name);
                if (info) {
                    std::cout << "  " << info->name << " - " << info->description << "\n";
                }
            }
            std::cout << "\nType 'help <command>' for detailed information.\n";
        }
        else {
            const auto* info = registry_->get_command(ctx.args[0]);
            if (info) {
                std::cout << "Command: " << info->name << "\n";
                std::cout << "Description: " << info->description << "\n";
                std::cout << "Usage: " << info->usage << "\n";
            }
            else {
                std::cout << "Unknown command: " << ctx.args[0] << "\n";
                return 1;
            }
        }
        return 0;
    };
    registry_->register_command(help_cmd);

    // Version command
    CommandInfo version_cmd;
    version_cmd.name = "version";
    version_cmd.description = "Display version information";
    version_cmd.usage = "version";
    version_cmd.handler = [](const CommandContext&) -> int {
        std::cout << "CustomOS Shell v1.0.0\n";
        std::cout << "Build date: " << __DATE__ << " " << __TIME__ << "\n";
        return 0;
    };
    registry_->register_command(version_cmd);

    // Echo command
    CommandInfo echo_cmd;
    echo_cmd.name = "echo";
    echo_cmd.description = "Display a line of text";
    echo_cmd.usage = "echo <text>";
    echo_cmd.handler = [](const CommandContext& ctx) -> int {
        for (size_t i = 0; i < ctx.args.size(); ++i) {
            std::cout << ctx.args[i];
            if (i < ctx.args.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
        return 0;
    };
    registry_->register_command(echo_cmd);

    // Whoami command
    CommandInfo whoami_cmd;
    whoami_cmd.name = "whoami";
    whoami_cmd.description = "Display current user";
    whoami_cmd.usage = "whoami";
    whoami_cmd.handler = [](const CommandContext& ctx) -> int {
        std::string user = ctx.current_user;
        if (user.empty()) {
            std::cout << "Not logged in (guest)\n";
        }
        else {
            std::cout << user << "\n";
        }
        return 0;
    };
    registry_->register_command(whoami_cmd);

    // Login command
    CommandInfo login_cmd;
    login_cmd.name = "login";
    login_cmd.description = "Login as a user";
    login_cmd.usage = "login <username>";
    login_cmd.handler = [](const CommandContext& ctx) -> int {
        if (ctx.args.empty()) {
            std::cout << "Usage: login <username>\n";
            return 1;
        }
        // TODO: Implement actual login
        std::cout << "Login functionality not yet implemented\n";
        return 0;
    };
    registry_->register_command(login_cmd);

    // Additional commands will be registered by respective modules
    // TODO: Implement VFS, Vault, Network, Plugin, and Logging commands
}

} // namespace core
} // namespace customos
