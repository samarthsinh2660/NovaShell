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
#ifdef _WIN32
#include <conio.h> // For Windows password input
#endif

namespace customos {
namespace core {

#ifdef _WIN32
// Windows password input function
std::string get_hidden_password() {
    std::string password;
    char ch;
    
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b"; // Move cursor back, print space, move cursor back
            }
        } else if (ch != '\0') { // Regular character
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}
#endif

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
        std::cout << "NovaShell v1.0.0\n";
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
            std::cout << "Available users: admin (password: admin)\n";
            return 1;
        }

        std::string username = ctx.args[0];

        // Check if user exists first
        if (!auth::Authentication::instance().user_exists(username)) {
            std::cout << "User '" << username << "' does not exist.\n";
            std::cout << "Available users: admin (password: admin)\n";
            std::cout << "To create a new user, use: create-user <username> <password>\n";
            return 1;
        }

        // Prompt for password
        std::cout << "Password for " << username << ": ";
#ifdef _WIN32
        std::string password = get_hidden_password();
#else
        std::string password;
        std::getline(std::cin, password);
        // Remove trailing whitespace/newlines
        password.erase(password.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

        if (auth::Authentication::instance().login(username, password)) {
            std::cout << "Login successful! Welcome, " << username << ".\n";
            return 0;
        } else {
            std::cout << "Login failed. Incorrect password.\n";
            return 1;
        }
    };
    registry_->register_command(login_cmd);

    // Create user command
    CommandInfo create_user_cmd;
    create_user_cmd.name = "create-user";
    create_user_cmd.description = "Create a new user account";
    create_user_cmd.usage = "create-user <username> <password>";
    create_user_cmd.handler = [](const CommandContext& ctx) -> int {
        if (ctx.args.size() < 2) {
            std::cout << "Usage: create-user <username> <password>\n";
            return 1;
        }

        std::string username = ctx.args[0];
        std::string password = ctx.args[1];

        // Check if user already exists
        if (auth::Authentication::instance().user_exists(username)) {
            std::cout << "User '" << username << "' already exists.\n";
            return 1;
        }

        // Create user with USER role by default
        if (auth::Authentication::instance().create_user(username, password, auth::UserRole::USER)) {
            std::cout << "User '" << username << "' created successfully!\n";
            std::cout << "You can now login with: login " << username << "\n";
            return 0;
        } else {
            std::cout << "Failed to create user.\n";
            return 1;
        }
    };
    registry_->register_command(create_user_cmd);

    // Logout command
    CommandInfo logout_cmd;
    logout_cmd.name = "logout";
    logout_cmd.description = "Logout current user";
    logout_cmd.usage = "logout";
    logout_cmd.handler = [](const CommandContext& ctx) -> int {
        (void)ctx; // Suppress unused parameter warning
        if (auth::Authentication::instance().is_logged_in()) {
            std::string current_user = auth::Authentication::instance().get_current_user();
            auth::Authentication::instance().logout();
            std::cout << "Logged out successfully. Goodbye, " << current_user << "!\n";
        } else {
            std::cout << "Not currently logged in.\n";
        }
        return 0;
    };
    registry_->register_command(logout_cmd);

    // Vault commands - Password management
    // Vault initialization
    CommandInfo vault_init_cmd;
    vault_init_cmd.name = "vault-init";
    vault_init_cmd.description = "Initialize password vault with master password";
    vault_init_cmd.usage = "vault-init";
    vault_init_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault is already initialized.\n";
            return 1;
        }

        std::cout << "Initialize password vault\n";
        std::cout << "Choose a strong master password: ";
#ifdef _WIN32
        std::string master_pass = get_hidden_password();
#else
        std::string master_pass;
        std::getline(std::cin, master_pass);
        master_pass.erase(master_pass.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

        if (master_pass.length() < 8) {
            std::cout << "Master password must be at least 8 characters long.\n";
            return 1;
        }

        std::cout << "Confirm master password: ";
#ifdef _WIN32
        std::string confirm_pass = get_hidden_password();
#else
        std::string confirm_pass;
        std::getline(std::cin, confirm_pass);
        confirm_pass.erase(confirm_pass.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

        if (master_pass != confirm_pass) {
            std::cout << "Passwords do not match.\n";
            return 1;
        }

        if (vault::PasswordManager::instance().initialize(master_pass)) {
            std::cout << "Vault initialized successfully!\n";
            return 0;
        } else {
            std::cout << "Failed to initialize vault.\n";
            return 1;
        }
    };
    registry_->register_command(vault_init_cmd);

    // Vault unlock
    CommandInfo vault_unlock_cmd;
    vault_unlock_cmd.name = "vault-unlock";
    vault_unlock_cmd.description = "Unlock password vault";
    vault_unlock_cmd.usage = "vault-unlock";
    vault_unlock_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is already unlocked.\n";
            return 0;
        }

        std::cout << "Enter master password: ";
#ifdef _WIN32
        std::string master_pass = get_hidden_password();
#else
        std::string master_pass;
        std::getline(std::cin, master_pass);
        master_pass.erase(master_pass.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

        if (vault::PasswordManager::instance().unlock(master_pass)) {
            std::cout << "Vault unlocked successfully!\n";
            return 0;
        } else {
            std::cout << "Incorrect master password.\n";
            return 1;
        }
    };
    registry_->register_command(vault_unlock_cmd);

    // Vault lock
    CommandInfo vault_lock_cmd;
    vault_lock_cmd.name = "vault-lock";
    vault_lock_cmd.description = "Lock password vault";
    vault_lock_cmd.usage = "vault-lock";
    vault_lock_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        vault::PasswordManager::instance().lock();
        std::cout << "Vault locked.\n";
        return 0;
    };
    registry_->register_command(vault_lock_cmd);

    // Add password
    CommandInfo vault_add_cmd;
    vault_add_cmd.name = "vault-add";
    vault_add_cmd.description = "Add a new password entry";
    vault_add_cmd.usage = "vault-add";
    vault_add_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is locked. Use 'vault-unlock' first.\n";
            return 1;
        }

        vault::PasswordEntry entry;

        std::cout << "Service name: ";
        std::getline(std::cin, entry.service);

        std::cout << "Username: ";
        std::getline(std::cin, entry.username);

        std::cout << "Password: ";
#ifdef _WIN32
        entry.password = get_hidden_password();
#else
        std::getline(std::cin, entry.password);
        entry.password.erase(entry.password.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

        std::cout << "URL (optional): ";
        std::getline(std::cin, entry.url);

        std::cout << "Notes (optional): ";
        std::getline(std::cin, entry.notes);

        entry.created = time(nullptr);
        entry.modified = entry.created;

        if (vault::PasswordManager::instance().add_password(entry)) {
            std::cout << "Password added successfully!\n";
            return 0;
        } else {
            std::cout << "Failed to add password.\n";
            return 1;
        }
    };
    registry_->register_command(vault_add_cmd);

    // List passwords
    CommandInfo vault_list_cmd;
    vault_list_cmd.name = "vault-list";
    vault_list_cmd.description = "List all password entries";
    vault_list_cmd.usage = "vault-list";
    vault_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is locked. Use 'vault-unlock' first.\n";
            return 1;
        }

        auto passwords = vault::PasswordManager::instance().list_passwords();
        if (passwords.empty()) {
            std::cout << "No passwords stored.\n";
            return 0;
        }

        std::cout << "Stored passwords:\n";
        std::cout << "================\n";
        for (size_t i = 0; i < passwords.size(); ++i) {
            std::cout << i + 1 << ". " << passwords[i].service 
                     << " (" << passwords[i].username << ")\n";
        }
        return 0;
    };
    registry_->register_command(vault_list_cmd);

    // Get password
    CommandInfo vault_get_cmd;
    vault_get_cmd.name = "vault-get";
    vault_get_cmd.description = "Retrieve a password entry";
    vault_get_cmd.usage = "vault-get <service>";
    vault_get_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is locked. Use 'vault-unlock' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: vault-get <service>\n";
            return 1;
        }

        std::string service = ctx.args[0];
        auto password_opt = vault::PasswordManager::instance().get_password(service);

        if (password_opt) {
            const auto& entry = *password_opt;
            std::cout << "Service: " << entry.service << "\n";
            std::cout << "Username: " << entry.username << "\n";
            std::cout << "Password: " << entry.password << "\n";
            if (!entry.url.empty()) {
                std::cout << "URL: " << entry.url << "\n";
            }
            if (!entry.notes.empty()) {
                std::cout << "Notes: " << entry.notes << "\n";
            }
            return 0;
        } else {
            std::cout << "Password not found for service: " << service << "\n";
            return 1;
        }
    };
    registry_->register_command(vault_get_cmd);

    // Delete password
    CommandInfo vault_delete_cmd;
    vault_delete_cmd.name = "vault-delete";
    vault_delete_cmd.description = "Delete a password entry";
    vault_delete_cmd.usage = "vault-delete <service>";
    vault_delete_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is locked. Use 'vault-unlock' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: vault-delete <service>\n";
            return 1;
        }

        std::string service = ctx.args[0];
        if (vault::PasswordManager::instance().delete_password(service)) {
            std::cout << "Password deleted successfully!\n";
            return 0;
        } else {
            std::cout << "Failed to delete password or password not found.\n";
            return 1;
        }
    };
    registry_->register_command(vault_delete_cmd);

    // Search passwords
    CommandInfo vault_search_cmd;
    vault_search_cmd.name = "vault-search";
    vault_search_cmd.description = "Search password entries";
    vault_search_cmd.usage = "vault-search <query>";
    vault_search_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault not initialized. Use 'vault-init' first.\n";
            return 1;
        }

        if (!vault::PasswordManager::instance().is_unlocked()) {
            std::cout << "Vault is locked. Use 'vault-unlock' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: vault-search <query>\n";
            return 1;
        }

        std::string query = ctx.args[0];
        auto results = vault::PasswordManager::instance().search_passwords(query);

        if (results.empty()) {
            std::cout << "No passwords found matching: " << query << "\n";
            return 0;
        }

        std::cout << "Search results for '" << query << "':\n";
        std::cout << "==================================\n";
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << i + 1 << ". " << results[i].service 
                     << " (" << results[i].username << ")\n";
        }
        return 0;
    };
    registry_->register_command(vault_search_cmd);
}

} // namespace core
} // namespace customos
