#include "core/command_processor.h"
#include "core/command_registry.h"
#include "auth/authentication.h"
#include "vfs/virtual_filesystem.h"
#include "vault/password_manager.h"
#include "git/git_manager.h"
#include "network/packet_analyzer.h"
#include "containers/container_manager.h"
#include "monitor/system_monitor.h"
#include "plugins/plugin_manager.h"
#include "scripting/script_engine.h"
#include "logging/logger.h"
#include "notes/snippet_manager.h"
#include "scheduler/task_scheduler.h"
#include "database/db_manager.h"
#include "p2p/file_sharing.h"
#include "ai/command_suggester.h"  // Temporarily disabled - requires libcurl
#include "remote/ssh_server.h"
#include "ui/theme_manager.h"
// #include "voice/voice_commander.h"  // Temporarily disabled
#include "analytics/dashboard.h"     // Analytics dashboard
#include "env/environment_manager.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
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
    help_cmd.usage = "help [category|command]";
    help_cmd.handler = [this](const CommandContext& ctx) -> int {
        if (ctx.args.empty()) {
            // Show category selection
            std::cout << "🆘 NovaShell Help System\n";
            std::cout << "========================\n\n";
            std::cout << "Available Categories:\n\n";
            std::cout << "1. 🔐 Authentication    - User login and account management\n";
            std::cout << "2. 🔑 Password Vault    - Secure password storage and management\n";
            std::cout << "3. 🔀 Git Integration   - Version control operations\n";
            std::cout << "4. 🌐 Network Tools     - Packet analysis and monitoring\n";
            std::cout << "5. 🐳 Container Mgmt    - Docker/Podman operations\n";
            std::cout << "6. 📊 System Monitor    - Real-time system metrics\n";
            std::cout << "7. 📝 Notes & Snippets  - Knowledge management\n";
            std::cout << "8. ⏰ Task Scheduling   - Automated task management\n";
            std::cout << "9. 🤖 AI Features       - Intelligent command suggestions\n";
            std::cout << "10. 🌐 Remote Access    - SSH server management\n";
            std::cout << "11. 🎨 Themes           - Visual customization\n";
            // std::cout << "12. 🎤 Voice Commands   - Speech-to-text interface\n";
            std::cout << "12. 📊 Analytics        - Dashboard and metrics\n";
            std::cout << "13. 🔧 Environment Mgr  - Development environment switching\n";
            std::cout << "14. 🛠️ File System      - File and directory operations\n";
            std::cout << "15. 🏗️ Virtual FS       - Cross-platform file management\n";
            std::cout << "16. 💾 Database         - SQL database operations\n";
            std::cout << "17. 📦 P2P Sharing      - LAN file sharing\n";
            std::cout << "18. 🔌 Plugins          - Extension system\n";
            std::cout << "19. ⚙️ Scripting        - Script execution\n";
            std::cout << "20. 📋 Logging          - System logs and audit trails\n";
            std::cout << "21. 🛠️ Utilities        - General-purpose commands\n\n";
            std::cout << "📖 For detailed guide, see: COMMAND_REFERENCE.md\n\n";
            std::cout << "Usage:\n";
            std::cout << "  help <number>     - Show commands in category\n";
            std::cout << "  help <category>   - Show commands in category\n";
            std::cout << "  help <command>    - Show detailed command help\n";
            std::cout << "  help all          - Show all commands\n\n";
            return 0;
        }

        std::string arg = ctx.args[0];

        // Handle category selection by number
        if (arg == "1" || arg == "authentication" || arg == "auth") {
            show_category_help("🔐 Authentication", {
                {"login <username>", "Authenticate as a user to access NovaShell features"},
                {"logout", "End current user session"},
                {"create-user <username> <password>", "Create a new user account"},
                {"whoami", "Display current logged-in user"}
            });
        }
        else if (arg == "2" || arg == "vault" || arg == "password") {
            show_category_help("🔑 Password Vault", {
                {"vault-init", "Initialize password vault with master password"},
                {"vault-unlock", "Unlock vault for password operations"},
                {"vault-lock", "Lock vault to prevent access"},
                {"vault-add", "Add a new password entry"},
                {"vault-list", "List all stored passwords"},
                {"vault-get <service>", "Retrieve password for a specific service"},
                {"vault-delete <service>", "Remove a password entry"},
                {"vault-search <query>", "Search passwords by service name or username"}
            });
        }
        else if (arg == "3" || arg == "git") {
            show_category_help("🔀 Git Integration", {
                {"git-status", "Show current repository status"},
                {"git-add <file> [--all]", "Stage files for commit"},
                {"git-commit <message>", "Commit staged changes"},
                {"git-log [limit]", "Show commit history"},
                {"git-branch [name]", "List branches or create new branch"},
                {"git-checkout <branch>", "Switch to different branch"},
                {"git-pull [remote] [branch]", "Fetch and merge from remote"},
                {"git-push [remote] [branch]", "Push commits to remote repository"}
            });
        }
        else if (arg == "4" || arg == "network" || arg == "net") {
            show_category_help("🌐 Network Tools", {
                {"net-interfaces", "List available network interfaces"},
                {"net-stats", "Show network traffic statistics"},
                {"net-capture [interface]", "Start packet capture on interface"},
                {"net-stop", "Stop current packet capture"},
                {"net-packets [count]", "Show recently captured packets"},
                {"net-protocols", "Show protocol distribution statistics"}
            });
        }
        else if (arg == "5" || arg == "docker" || arg == "container") {
            show_category_help("🐳 Container Management", {
                {"docker-init", "Initialize container runtime connection"},
                {"docker-ps [--all]", "List containers"},
                {"docker-run <image> <name>", "Create and start a new container"},
                {"docker-stop <container>", "Stop a running container"},
                {"docker-start <container>", "Start a stopped container"},
                {"docker-rm [--force] <container>", "Remove a container"},
                {"docker-images", "List container images"},
                {"docker-pull <image>", "Pull a container image"}
            });
        }
        else if (arg == "6" || arg == "monitor" || arg == "system") {
            show_category_help("📊 System Monitoring", {
                {"monitor-cpu", "Show CPU information and usage"},
                {"monitor-memory", "Show memory usage statistics"},
                {"monitor-disk", "Show disk usage information"},
                {"monitor-network", "Show network interface information"},
                {"monitor-processes [--cpu]", "Show running processes"},
                {"monitor-system", "Show general system information"}
            });
        }
        else if (arg == "7" || arg == "notes" || arg == "snippets") {
            show_category_help("📝 Notes & Snippets", {
                {"note-add <title>", "Add a new note"},
                {"note-list [category]", "List notes, optionally filtered by category"},
                {"note-get <id>", "View a specific note"},
                {"snippet-add <title> <language>", "Add a code snippet"},
                {"snippet-list [language]", "List code snippets, optionally filtered by language"},
                {"snippet-get <id>", "View a specific code snippet"}
            });
        }
        else if (arg == "8" || arg == "scheduler" || arg == "task") {
            show_category_help("⏰ Task Scheduling", {
                {"scheduler-init", "Initialize the task scheduler"},
                {"task-schedule <title> <command> <minutes>", "Schedule a command to run after specified minutes"},
                {"task-list [category]", "List scheduled tasks"},
                {"task-cancel <task_id>", "Cancel a scheduled task"},
                {"task-run <task_id>", "Execute a scheduled task immediately"},
                {"remind-add <title> <minutes>", "Set a reminder for specified minutes from now"},
                {"remind-list", "Show active reminders"}
            });
        }
        else if (arg == "9" || arg == "ai") {
            show_category_help("🤖 AI Features", {
                {"ai-init <api_key>", "Initialize AI suggestions with Gemini API key"},
                {"ai-suggest [context]", "Get AI-powered command suggestions"}
            });
        }
        else if (arg == "10" || arg == "remote" || arg == "ssh") {
            show_category_help("🌐 Remote Access", {
                {"ssh-start [port]", "Start SSH server for remote connections"},
                {"ssh-stop", "Stop the SSH server"},
                {"ssh-connections", "Show active SSH connections"}
            });
        }
        else if (arg == "11" || arg == "theme" || arg == "themes") {
            show_category_help("🎨 Themes", {
                {"theme-list", "List available themes"},
                {"theme-set <theme>", "Apply a specific theme"},
                {"theme-create <name>", "Create a custom theme"}
            });
        }
        // Voice commands temporarily disabled
        /*
        else if (arg == "12" || arg == "voice") {
            show_category_help("🎤 Voice Commands", {
                {"voice-config [api_key]", "Configure voice recognition"},
                {"voice-start", "Start voice command recognition"},
                {"voice-stop", "Stop voice command recognition"}
            });
        }
        */
        else if (arg == "12" || arg == "analytics" || arg == "dashboard") {
            show_category_help("📊 Analytics", {
                {"dashboard [name]", "Show analytics dashboard"},
                {"analytics [metric]", "Show analytics metrics or overview"}
            });
        }
        else if (arg == "13" || arg == "environment" || arg == "env") {
            show_category_help("🔧 Environment Manager", {
                {"env-list", "List available environment profiles"},
                {"env-create <name> [description]", "Create a new environment profile"},
                {"env-switch <profile>", "Switch to a different environment profile"}
            });
        }
        else if (arg == "14" || arg == "filesystem" || arg == "files") {
            show_category_help("🛠️ File System", {
                {"file-list [directory]", "List files and directories"}
            });
        }
        else if (arg == "15" || arg == "vfs") {
            show_category_help("🏗️ Virtual File System", {
                {"vfs-mount <device> <mount_point> [type]", "Mount a filesystem"}
            });
        }
        else if (arg == "16" || arg == "database" || arg == "db") {
            show_category_help("💾 Database", {
                {"db-connect <type> <name> <connection_details>", "Connect to a database"}
            });
        }
        else if (arg == "17" || arg == "p2p") {
            show_category_help("📦 P2P Sharing", {
                {"p2p-share <file> [--public]", "Share a file via P2P on the local network"}
            });
        }
        else if (arg == "18" || arg == "plugin" || arg == "plugins") {
            show_category_help("🔌 Plugins", {
                {"plugin-list", "List loaded plugins and their capabilities"}
            });
        }
        else if (arg == "19" || arg == "scripting" || arg == "script") {
            show_category_help("⚙️ Scripting", {
                {"script-run <script_file>", "Execute a script file"}
            });
        }
        else if (arg == "20" || arg == "logging" || arg == "logs") {
            show_category_help("📋 Logging", {
                {"log-show [count]", "Show recent system logs and audit entries"}
            });
        }
        else if (arg == "21" || arg == "utilities" || arg == "util") {
            show_category_help("🛠️ Utilities", {
                {"help [category|command]", "Show help for categories or specific commands"},
                {"version", "Show NovaShell version information"},
                {"echo <text>", "Display text or variables"}
            });
        }
        else if (arg == "all") {
            // Show all commands
            std::cout << "All Available Commands:\n";
            std::cout << "======================\n\n";
            auto commands = registry_->list_commands();
            std::sort(commands.begin(), commands.end());
            for (const auto& cmd_name : commands) {
                const auto* info = registry_->get_command(cmd_name);
                if (info) {
                    std::cout << "  " << info->name << " - " << info->description << "\n";
                }
            }
            std::cout << "\nTotal commands: " << commands.size() << "\n";
            std::cout << "📖 See COMMAND_REFERENCE.md for detailed usage examples.\n";
        }
        else {
            // Try to find specific command
            const auto* info = registry_->get_command(arg);
            if (info) {
                std::cout << "Command: " << info->name << "\n";
                std::cout << "Description: " << info->description << "\n";
                std::cout << "Usage: " << info->usage << "\n";
                std::cout << "\n📖 For examples and detailed usage, see COMMAND_REFERENCE.md\n";
            }
            else {
                std::cout << "Unknown category or command: " << arg << "\n";
                std::cout << "Type 'help' to see available categories.\n";
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
    vault_init_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use the vault.\n";
            return 1;
        }

        if (vault::PasswordManager::instance().is_initialized()) {
            std::cout << "Vault is already initialized.\n";
            return 1;
        }

        const int MAX_ATTEMPTS = 3;
        int attempts = 0;
        bool password_valid = false;
        std::string master_pass;
        std::string confirm_pass;

        while (attempts < MAX_ATTEMPTS && !password_valid) {
            attempts++;
            std::cout << "Initialize password vault\n";
            if (attempts > 1) {
                std::cout << "(Attempt " << attempts << " of " << MAX_ATTEMPTS << ")\n";
            }
            std::cout << "Choose a strong master password: ";
            
#ifdef _WIN32
            master_pass = get_hidden_password();
#else
            std::getline(std::cin, master_pass);
            master_pass.erase(master_pass.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

            if (master_pass.length() < 8) {
                std::cout << "Master password must be at least 8 characters long.\n";
                if (attempts < MAX_ATTEMPTS) {
                    std::cout << "Please try again.\n\n";
                    continue;
                } else {
                    std::cout << "Maximum attempts reached. Vault initialization cancelled.\n";
                    return 1;
                }
            }

            std::cout << "Confirm master password: ";
#ifdef _WIN32
            confirm_pass = get_hidden_password();
#else
            std::getline(std::cin, confirm_pass);
            confirm_pass.erase(confirm_pass.find_last_not_of(" \t\n\r\f\v") + 1);
#endif

            if (master_pass != confirm_pass) {
                std::cout << "Passwords do not match.\n";
                if (attempts < MAX_ATTEMPTS) {
                    std::cout << "Please try again.\n\n";
                    continue;
                } else {
                    std::cout << "Maximum attempts reached. Vault initialization cancelled.\n";
                    return 1;
                }
            }

            password_valid = true;
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
    vault_unlock_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
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
    vault_lock_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
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
    vault_add_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
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
    vault_list_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
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

    // Git commands - Version control operations
    // Git status
    CommandInfo git_status_cmd;
    git_status_cmd.name = "git-status";
    git_status_cmd.description = "Show git repository status";
    git_status_cmd.usage = "git-status";
    git_status_cmd.handler = []([[maybe_unused]] const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        auto status = git::GitManager::instance().status();
        if (status.empty()) {
            std::cout << "Working tree clean.\n";
            return 0;
        }

        std::cout << "Git status:\n";
        for (const auto& file : status) {
            std::string status_str;
            switch (file.status) {
                case git::FileStatus::MODIFIED: status_str = "modified"; break;
                case git::FileStatus::ADDED: status_str = "added"; break;
                case git::FileStatus::DELETED: status_str = "deleted"; break;
                case git::FileStatus::RENAMED: status_str = "renamed"; break;
                case git::FileStatus::COPIED: status_str = "copied"; break;
                case git::FileStatus::UNTRACKED: status_str = "untracked"; break;
                case git::FileStatus::UNCHANGED: status_str = "unchanged"; break;
            }
            std::cout << "  " << status_str << ": " << file.path << "\n";
        }
        return 0;
    };
    registry_->register_command(git_status_cmd);

    // Git add
    CommandInfo git_add_cmd;
    git_add_cmd.name = "git-add";
    git_add_cmd.description = "Add files to staging area";
    git_add_cmd.usage = "git-add <file> or git-add --all";
    git_add_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: git-add <file> or git-add --all\n";
            return 1;
        }

        bool success;
        if (ctx.args[0] == "--all") {
            success = git::GitManager::instance().add_all();
            std::cout << (success ? "Added all files to staging area.\n" : "Failed to add files.\n");
        } else {
            success = git::GitManager::instance().add(ctx.args[0]);
            std::cout << (success ? "Added " + ctx.args[0] + " to staging area.\n" : "Failed to add file.\n");
        }
        return success ? 0 : 1;
    };
    registry_->register_command(git_add_cmd);

    // Git commit
    CommandInfo git_commit_cmd;
    git_commit_cmd.name = "git-commit";
    git_commit_cmd.description = "Commit staged changes";
    git_commit_cmd.usage = "git-commit <message>";
    git_commit_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: git-commit <message>\n";
            return 1;
        }

        std::string message = ctx.args[0];
        for (size_t i = 1; i < ctx.args.size(); ++i) {
            message += " " + ctx.args[i];
        }

        bool success = git::GitManager::instance().commit(message);
        std::cout << (success ? "Changes committed successfully.\n" : "Failed to commit changes.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(git_commit_cmd);

    // Git log
    CommandInfo git_log_cmd;
    git_log_cmd.name = "git-log";
    git_log_cmd.description = "Show commit history";
    git_log_cmd.usage = "git-log [limit]";
    git_log_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        int limit = 10;
        if (!ctx.args.empty()) {
            try {
                limit = std::stoi(ctx.args[0]);
            } catch (...) {
                limit = 10;
            }
        }

        auto commits = git::GitManager::instance().log(limit);
        if (commits.empty()) {
            std::cout << "No commits found.\n";
            return 0;
        }

        for (const auto& commit : commits) {
            std::cout << "Commit: " << commit.hash.substr(0, 8) << "\n";
            std::cout << "Author: " << commit.author << " <" << commit.email << ">\n";
            std::cout << "Date: " << std::ctime(&commit.timestamp);
            std::cout << "Message: " << commit.message << "\n\n";
        }
        return 0;
    };
    registry_->register_command(git_log_cmd);

    // Git branch
    CommandInfo git_branch_cmd;
    git_branch_cmd.name = "git-branch";
    git_branch_cmd.description = "List or create branches";
    git_branch_cmd.usage = "git-branch or git-branch <name>";
    git_branch_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            // List branches
            auto branches = git::GitManager::instance().list_branches(false);
            for (const auto& branch : branches) {
                std::cout << (branch.is_current ? "* " : "  ") << branch.name;
                if (!branch.upstream.empty()) {
                    std::cout << " -> " << branch.upstream;
                    if (branch.commits_ahead > 0 || branch.commits_behind > 0) {
                        std::cout << " [" << branch.commits_ahead << " ahead, " 
                                 << branch.commits_behind << " behind]";
                    }
                }
                std::cout << "\n";
            }
        } else {
            // Create branch
            bool success = git::GitManager::instance().create_branch(ctx.args[0]);
            std::cout << (success ? "Branch '" + ctx.args[0] + "' created.\n" 
                                : "Failed to create branch.\n");
            return success ? 0 : 1;
        }
        return 0;
    };
    registry_->register_command(git_branch_cmd);

    // Git checkout
    CommandInfo git_checkout_cmd;
    git_checkout_cmd.name = "git-checkout";
    git_checkout_cmd.description = "Switch branches or restore files";
    git_checkout_cmd.usage = "git-checkout <branch>";
    git_checkout_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: git-checkout <branch>\n";
            return 1;
        }

        bool success = git::GitManager::instance().checkout(ctx.args[0]);
        std::cout << (success ? "Switched to " + ctx.args[0] + ".\n" 
                            : "Failed to checkout.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(git_checkout_cmd);

    // Git pull
    CommandInfo git_pull_cmd;
    git_pull_cmd.name = "git-pull";
    git_pull_cmd.description = "Fetch and merge from remote";
    git_pull_cmd.usage = "git-pull [remote] [branch]";
    git_pull_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        std::string remote = ctx.args.size() > 0 ? ctx.args[0] : "origin";
        std::string branch = ctx.args.size() > 1 ? ctx.args[1] : "";

        bool success = git::GitManager::instance().pull(remote, branch);
        std::cout << (success ? "Pull completed successfully.\n" : "Pull failed.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(git_pull_cmd);

    // Git push
    CommandInfo git_push_cmd;
    git_push_cmd.name = "git-push";
    git_push_cmd.description = "Push commits to remote";
    git_push_cmd.usage = "git-push [remote] [branch]";
    git_push_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use git commands.\n";
            return 1;
        }

        if (!git::GitManager::instance().is_repository()) {
            std::cout << "Not a git repository.\n";
            return 1;
        }

        std::string remote = ctx.args.size() > 0 ? ctx.args[0] : "origin";
        std::string branch = ctx.args.size() > 1 ? ctx.args[1] : "";

        bool success = git::GitManager::instance().push(remote, branch);
        std::cout << (success ? "Push completed successfully.\n" : "Push failed.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(git_push_cmd);

    // Network commands - Packet analysis and monitoring
    // Network interfaces
    CommandInfo net_interfaces_cmd;
    net_interfaces_cmd.name = "net-interfaces";
    net_interfaces_cmd.description = "List available network interfaces";
    net_interfaces_cmd.usage = "net-interfaces";
    net_interfaces_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto interfaces = network::PacketAnalyzer::instance().list_interfaces();
        if (interfaces.empty()) {
            std::cout << "No network interfaces found.\n";
            return 1;
        }

        std::cout << "Available network interfaces:\n";
        for (size_t i = 0; i < interfaces.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << interfaces[i] << "\n";
        }
        return 0;
    };
    registry_->register_command(net_interfaces_cmd);

    // Network statistics
    CommandInfo net_stats_cmd;
    net_stats_cmd.name = "net-stats";
    net_stats_cmd.description = "Show network statistics";
    net_stats_cmd.usage = "net-stats";
    net_stats_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto stats = network::PacketAnalyzer::instance().get_statistics();
        std::cout << "Network Statistics:\n";
        std::cout << "==================\n";
        std::cout << "Total packets: " << stats.total_packets << "\n";
        std::cout << "Total bytes: " << stats.total_bytes << "\n";
        std::cout << "TCP packets: " << stats.tcp_packets << "\n";
        std::cout << "UDP packets: " << stats.udp_packets << "\n";
        std::cout << "ICMP packets: " << stats.icmp_packets << "\n";
        std::cout << "Other packets: " << stats.other_packets << "\n";

        if (!stats.top_talkers.empty()) {
            std::cout << "\nTop talkers:\n";
            for (const auto& pair : stats.top_talkers) {
                std::cout << "  " << pair.first << ": " << pair.second << " bytes\n";
            }
        }
        return 0;
    };
    registry_->register_command(net_stats_cmd);

    // Start packet capture
    CommandInfo net_capture_cmd;
    net_capture_cmd.name = "net-capture";
    net_capture_cmd.description = "Start packet capture on interface";
    net_capture_cmd.usage = "net-capture [interface]";
    net_capture_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        std::string interface = ctx.args.empty() ? 
            network::PacketAnalyzer::instance().get_default_interface() : ctx.args[0];

        network::PacketFilter filter;
        bool success = network::PacketAnalyzer::instance().start_capture(interface, filter);

        if (success) {
            std::cout << "Started packet capture on interface: " << interface << "\n";
            std::cout << "Press Ctrl+C to stop capture.\n";
            return 0;
        } else {
            std::cout << "Failed to start packet capture.\n";
            return 1;
        }
    };
    registry_->register_command(net_capture_cmd);

    // Stop packet capture
    CommandInfo net_stop_cmd;
    net_stop_cmd.name = "net-stop";
    net_stop_cmd.description = "Stop packet capture";
    net_stop_cmd.usage = "net-stop";
    net_stop_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        network::PacketAnalyzer::instance().stop_capture();
        std::cout << "Packet capture stopped.\n";
        return 0;
    };
    registry_->register_command(net_stop_cmd);

    // Show recent packets
    CommandInfo net_packets_cmd;
    net_packets_cmd.name = "net-packets";
    net_packets_cmd.description = "Show recent captured packets";
    net_packets_cmd.usage = "net-packets [count]";
    net_packets_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        size_t count = 10;
        if (!ctx.args.empty()) {
            try {
                count = std::stoul(ctx.args[0]);
            } catch (...) {
                count = 10;
            }
        }

        auto packets = network::PacketAnalyzer::instance().get_packets(count);
        if (packets.empty()) {
            std::cout << "No packets captured yet.\n";
            return 0;
        }

        std::cout << "Recent packets:\n";
        std::cout << "===============\n";
        for (size_t i = 0; i < packets.size(); ++i) {
            const auto& packet = packets[i];
            std::string proto_str;
            switch (packet.protocol) {
                case network::Protocol::TCP: proto_str = "TCP"; break;
                case network::Protocol::UDP: proto_str = "UDP"; break;
                case network::Protocol::ICMP: proto_str = "ICMP"; break;
                case network::Protocol::HTTP: proto_str = "HTTP"; break;
                case network::Protocol::HTTPS: proto_str = "HTTPS"; break;
                default: proto_str = "Unknown"; break;
            }

            std::cout << i + 1 << ". " << proto_str << " "
                     << packet.src_ip << ":" << packet.src_port << " -> "
                     << packet.dst_ip << ":" << packet.dst_port << " ("
                     << packet.size << " bytes)\n";
        }
        return 0;
    };
    registry_->register_command(net_packets_cmd);

    // Protocol distribution
    CommandInfo net_protocols_cmd;
    net_protocols_cmd.name = "net-protocols";
    net_protocols_cmd.description = "Show protocol distribution";
    net_protocols_cmd.usage = "net-protocols";
    net_protocols_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use network commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto distribution = network::PacketAnalyzer::instance().get_protocol_distribution();
        if (distribution.empty()) {
            std::cout << "No protocol data available.\n";
            return 0;
        }

        std::cout << "Protocol Distribution:\n";
        std::cout << "=====================\n";
        for (const auto& pair : distribution) {
            std::string proto_str;
            switch (pair.first) {
                case network::Protocol::TCP: proto_str = "TCP"; break;
                case network::Protocol::UDP: proto_str = "UDP"; break;
                case network::Protocol::ICMP: proto_str = "ICMP"; break;
                case network::Protocol::HTTP: proto_str = "HTTP"; break;
                case network::Protocol::HTTPS: proto_str = "HTTPS"; break;
                default: proto_str = "Unknown"; break;
            }
            std::cout << "  " << proto_str << ": " << pair.second << " packets\n";
        }
        return 0;
    };
    registry_->register_command(net_protocols_cmd);

    // Container commands - Docker/Podman management
    // Initialize container runtime
    CommandInfo docker_init_cmd;
    docker_init_cmd.name = "docker-init";
    docker_init_cmd.description = "Initialize container runtime connection";
    docker_init_cmd.usage = "docker-init";
    docker_init_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        if (containers::ContainerManager::instance().initialize()) {
            std::cout << "Container runtime initialized successfully.\n";
            return 0;
        } else {
            std::cout << "Failed to initialize container runtime. Make sure Docker/Podman is running.\n";
            return 1;
        }
    };
    registry_->register_command(docker_init_cmd);

    // List containers
    CommandInfo docker_ps_cmd;
    docker_ps_cmd.name = "docker-ps";
    docker_ps_cmd.description = "List containers";
    docker_ps_cmd.usage = "docker-ps [--all]";
    docker_ps_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        bool all = false;
        if (!ctx.args.empty() && ctx.args[0] == "--all") {
            all = true;
        }

        auto containers = containers::ContainerManager::instance().list_containers(all);
        if (containers.empty()) {
            std::cout << "No containers found.\n";
            return 0;
        }

        std::cout << "CONTAINER ID\tIMAGE\t\tSTATUS\t\tPORTS\t\tNAMES\n";
        std::cout << "================================================================================\n";
        for (const auto& container : containers) {
            std::string status_str;
            switch (container.status) {
                case containers::ContainerStatus::RUNNING: status_str = "Running"; break;
                case containers::ContainerStatus::STOPPED: status_str = "Stopped"; break;
                case containers::ContainerStatus::PAUSED: status_str = "Paused"; break;
                case containers::ContainerStatus::RESTARTING: status_str = "Restarting"; break;
                case containers::ContainerStatus::DEAD: status_str = "Dead"; break;
                default: status_str = "Unknown"; break;
            }

            std::cout << container.id.substr(0, 12) << "\t"
                     << container.image.substr(0, 15) << "\t"
                     << status_str << "\t\t"
                     << (container.ports.empty() ? "-" : container.ports[0]) << "\t\t"
                     << container.name << "\n";
        }
        return 0;
    };
    registry_->register_command(docker_ps_cmd);

    // Create container
    CommandInfo docker_run_cmd;
    docker_run_cmd.name = "docker-run";
    docker_run_cmd.description = "Create and run a container";
    docker_run_cmd.usage = "docker-run <image> <name>";
    docker_run_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        if (ctx.args.size() < 2) {
            std::cout << "Usage: docker-run <image> <name>\n";
            return 1;
        }

        std::string image = ctx.args[0];
        std::string name = ctx.args[1];
        std::map<std::string, std::string> options;

        std::string container_id = containers::ContainerManager::instance().create_container(image, name, options);
        if (!container_id.empty()) {
            std::cout << "Container created with ID: " << container_id << "\n";

            // Try to start it
            if (containers::ContainerManager::instance().start_container(container_id)) {
                std::cout << "Container started successfully.\n";
                return 0;
            } else {
                std::cout << "Container created but failed to start.\n";
                return 1;
            }
        } else {
            std::cout << "Failed to create container.\n";
            return 1;
        }
    };
    registry_->register_command(docker_run_cmd);

    // Stop container
    CommandInfo docker_stop_cmd;
    docker_stop_cmd.name = "docker-stop";
    docker_stop_cmd.description = "Stop a running container";
    docker_stop_cmd.usage = "docker-stop <container>";
    docker_stop_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: docker-stop <container>\n";
            return 1;
        }

        bool success = containers::ContainerManager::instance().stop_container(ctx.args[0]);
        std::cout << (success ? "Container stopped successfully.\n" : "Failed to stop container.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(docker_stop_cmd);

    // Start container
    CommandInfo docker_start_cmd;
    docker_start_cmd.name = "docker-start";
    docker_start_cmd.description = "Start a stopped container";
    docker_start_cmd.usage = "docker-start <container>";
    docker_start_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: docker-start <container>\n";
            return 1;
        }

        bool success = containers::ContainerManager::instance().start_container(ctx.args[0]);
        std::cout << (success ? "Container started successfully.\n" : "Failed to start container.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(docker_start_cmd);

    // Remove container
    CommandInfo docker_rm_cmd;
    docker_rm_cmd.name = "docker-rm";
    docker_rm_cmd.description = "Remove a container";
    docker_rm_cmd.usage = "docker-rm [--force] <container>";
    docker_rm_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: docker-rm [--force] <container>\n";
            return 1;
        }

        bool force = false;
        std::string container_id;

        if (ctx.args[0] == "--force" && ctx.args.size() > 1) {
            force = true;
            container_id = ctx.args[1];
        } else {
            container_id = ctx.args[0];
        }

        bool success = containers::ContainerManager::instance().remove_container(container_id, force);
        std::cout << (success ? "Container removed successfully.\n" : "Failed to remove container.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(docker_rm_cmd);

    // List images
    CommandInfo docker_images_cmd;
    docker_images_cmd.name = "docker-images";
    docker_images_cmd.description = "List container images";
    docker_images_cmd.usage = "docker-images";
    docker_images_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto images = containers::ContainerManager::instance().list_images();
        if (images.empty()) {
            std::cout << "No images found.\n";
            return 0;
        }

        std::cout << "REPOSITORY\t\tTAG\t\tIMAGE ID\t\tSIZE\n";
        std::cout << "================================================================================\n";
        for (const auto& image : images) {
            std::cout << image.repository.substr(0, 15) << "\t\t"
                     << image.tag << "\t\t"
                     << image.id.substr(0, 12) << "\t\t"
                     << (image.size / 1024 / 1024) << "MB\n";
        }
        return 0;
    };
    registry_->register_command(docker_images_cmd);

    // Pull image
    CommandInfo docker_pull_cmd;
    docker_pull_cmd.name = "docker-pull";
    docker_pull_cmd.description = "Pull a container image";
    docker_pull_cmd.usage = "docker-pull <image>";
    docker_pull_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use container commands.\n";
            return 1;
        }

        if (!containers::ContainerManager::instance().is_initialized()) {
            std::cout << "Container runtime not initialized. Run 'docker-init' first.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: docker-pull <image>\n";
            return 1;
        }

        bool success = containers::ContainerManager::instance().pull_image(ctx.args[0]);
        std::cout << (success ? "Image pulled successfully.\n" : "Failed to pull image.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(docker_pull_cmd);

    // System monitoring commands
    // CPU info
    CommandInfo monitor_cpu_cmd;
    monitor_cpu_cmd.name = "monitor-cpu";
    monitor_cpu_cmd.description = "Show CPU information and usage";
    monitor_cpu_cmd.usage = "monitor-cpu";
    monitor_cpu_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto cpu_info = monitor::SystemMonitor::instance().get_cpu_info();
        std::cout << "CPU Information:\n";
        std::cout << "================\n";
        std::cout << "Model: " << cpu_info.model << "\n";
        std::cout << "Cores: " << cpu_info.cores << "\n";
        std::cout << "Threads: " << cpu_info.threads << "\n";
        std::cout << "Usage: " << cpu_info.usage_percent << "%\n";
        if (cpu_info.temperature > 0) {
            std::cout << "Temperature: " << cpu_info.temperature << "°C\n";
        }

        if (!cpu_info.core_usage.empty()) {
            std::cout << "\nPer-core usage:\n";
            for (size_t i = 0; i < cpu_info.core_usage.size(); ++i) {
                std::cout << "  Core " << i << ": " << cpu_info.core_usage[i] << "%\n";
            }
        }
        return 0;
    };
    registry_->register_command(monitor_cpu_cmd);

    // Memory info
    CommandInfo monitor_memory_cmd;
    monitor_memory_cmd.name = "monitor-memory";
    monitor_memory_cmd.description = "Show memory usage information";
    monitor_memory_cmd.usage = "monitor-memory";
    monitor_memory_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto mem_info = monitor::SystemMonitor::instance().get_memory_info();
        std::cout << "Memory Information:\n";
        std::cout << "==================\n";
        std::cout << "Total: " << (mem_info.total_bytes / 1024 / 1024) << " MB\n";
        std::cout << "Used: " << (mem_info.used_bytes / 1024 / 1024) << " MB\n";
        std::cout << "Available: " << (mem_info.available_bytes / 1024 / 1024) << " MB\n";
        std::cout << "Usage: " << mem_info.usage_percent << "%\n";

        if (mem_info.swap_total > 0) {
            std::cout << "Swap Total: " << (mem_info.swap_total / 1024 / 1024) << " MB\n";
            std::cout << "Swap Used: " << (mem_info.swap_used / 1024 / 1024) << " MB\n";
            std::cout << "Swap Usage: " << (mem_info.swap_used * 100.0 / mem_info.swap_total) << "%\n";
        }
        return 0;
    };
    registry_->register_command(monitor_memory_cmd);

    // Disk info
    CommandInfo monitor_disk_cmd;
    monitor_disk_cmd.name = "monitor-disk";
    monitor_disk_cmd.description = "Show disk usage information";
    monitor_disk_cmd.usage = "monitor-disk";
    monitor_disk_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto disks = monitor::SystemMonitor::instance().get_disk_info();
        if (disks.empty()) {
            std::cout << "No disk information available.\n";
            return 1;
        }

        std::cout << "Disk Information:\n";
        std::cout << "=================\n";
        std::cout << "Mount Point\t\tTotal\t\tUsed\t\tAvailable\tUsage\n";
        std::cout << "================================================================================\n";
        for (const auto& disk : disks) {
            std::cout << disk.mount_point << "\t\t"
                     << (disk.total_bytes / 1024 / 1024 / 1024) << "GB\t\t"
                     << (disk.used_bytes / 1024 / 1024 / 1024) << "GB\t\t"
                     << (disk.available_bytes / 1024 / 1024 / 1024) << "GB\t\t"
                     << disk.usage_percent << "%\n";
        }
        return 0;
    };
    registry_->register_command(monitor_disk_cmd);

    // Network info
    CommandInfo monitor_network_cmd;
    monitor_network_cmd.name = "monitor-network";
    monitor_network_cmd.description = "Show network interface information";
    monitor_network_cmd.usage = "monitor-network";
    monitor_network_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto networks = monitor::SystemMonitor::instance().get_network_info();
        if (networks.empty()) {
            std::cout << "No network information available.\n";
            return 1;
        }

        std::cout << "Network Interfaces:\n";
        std::cout << "===================\n";
        std::cout << "Interface\tIP Address\t\tSent\t\tReceived\tUpload\tDownload\n";
        std::cout << "================================================================================\n";
        for (const auto& net : networks) {
            std::cout << net.interface << "\t\t"
                     << net.ip_address << "\t\t"
                     << (net.bytes_sent / 1024 / 1024) << "MB\t\t"
                     << (net.bytes_received / 1024 / 1024) << "MB\t\t"
                     << (net.upload_speed / 1024) << "KB/s\t"
                     << (net.download_speed / 1024) << "KB/s\n";
        }
        return 0;
    };
    registry_->register_command(monitor_network_cmd);

    // Process list
    CommandInfo monitor_processes_cmd;
    monitor_processes_cmd.name = "monitor-processes";
    monitor_processes_cmd.description = "Show running processes";
    monitor_processes_cmd.usage = "monitor-processes [--cpu]";
    monitor_processes_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        bool sort_by_cpu = false;
        if (!ctx.args.empty() && ctx.args[0] == "--cpu") {
            sort_by_cpu = true;
        }

        auto processes = monitor::SystemMonitor::instance().get_processes(sort_by_cpu);
        if (processes.empty()) {
            std::cout << "No process information available.\n";
            return 1;
        }

        // Show top 20 processes
        size_t count = std::min(processes.size(), size_t(20));
        std::cout << "Top " << count << " Processes:\n";
        std::cout << "===================\n";
        std::cout << "PID\tCPU%\tMemory(MB)\tStatus\t\tName\n";
        std::cout << "================================================================================\n";
        for (size_t i = 0; i < count; ++i) {
            const auto& proc = processes[i];
            std::cout << proc.pid << "\t"
                     << proc.cpu_percent << "\t"
                     << (proc.memory_bytes / 1024 / 1024) << "\t\t"
                     << proc.status << "\t\t"
                     << proc.name.substr(0, 20) << "\n";
        }
        return 0;
    };
    registry_->register_command(monitor_processes_cmd);

    // System info
    CommandInfo monitor_system_cmd;
    monitor_system_cmd.name = "monitor-system";
    monitor_system_cmd.description = "Show general system information";
    monitor_system_cmd.usage = "monitor-system";
    monitor_system_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use monitoring commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto sys_info = monitor::SystemMonitor::instance().get_system_info();
        float uptime = monitor::SystemMonitor::instance().get_uptime_hours();
        int process_count = monitor::SystemMonitor::instance().get_process_count();
        int thread_count = monitor::SystemMonitor::instance().get_thread_count();

        std::cout << "System Information:\n";
        std::cout << "===================\n";
        for (const auto& pair : sys_info) {
            std::cout << pair.first << ": " << pair.second << "\n";
        }
        std::cout << "Uptime: " << uptime << " hours\n";
        std::cout << "Processes: " << process_count << "\n";
        std::cout << "Threads: " << thread_count << "\n";
        return 0;
    };
    registry_->register_command(monitor_system_cmd);

    // Notes & Snippets commands
    // Add note
    CommandInfo note_add_cmd;
    note_add_cmd.name = "note-add";
    note_add_cmd.description = "Add a new note";
    note_add_cmd.usage = "note-add <title>";
    note_add_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use notes.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: note-add <title>\n";
            return 1;
        }

        std::string title = ctx.args[0];

        std::cout << "Enter note content (press Enter twice to finish):\n";
        std::string content;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty() && !content.empty()) break;
            if (!content.empty()) content += "\n";
            content += line;
        }

        std::cout << "Category (optional): ";
        std::string category;
        std::getline(std::cin, category);

        std::cout << "Tags (comma-separated, optional): ";
        std::string tags_input;
        std::getline(std::cin, tags_input);

        notes::Tags tags;
        if (!tags_input.empty()) {
            std::stringstream ss(tags_input);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                // Trim whitespace
                tag.erase(tag.begin(), std::find_if(tag.begin(), tag.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
                tag.erase(std::find_if(tag.rbegin(), tag.rend(), [](int ch) {
                    return !std::isspace(ch);
                }).base(), tag.end());
                if (!tag.empty()) tags.push_back(tag);
            }
        }

        std::string note_id = notes::SnippetManager::instance().add_note(title, content, tags, category);
        if (!note_id.empty()) {
            std::cout << "Note added successfully with ID: " << note_id << "\n";
            return 0;
        } else {
            std::cout << "Failed to add note.\n";
            return 1;
        }
    };
    registry_->register_command(note_add_cmd);

    // List notes
    CommandInfo note_list_cmd;
    note_list_cmd.name = "note-list";
    note_list_cmd.description = "List all notes";
    note_list_cmd.usage = "note-list [category]";
    note_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use notes.\n";
            return 1;
        }

        std::string category;
        if (!ctx.args.empty()) {
            category = ctx.args[0];
        }

        auto notes = notes::SnippetManager::instance().list_notes(category);
        if (notes.empty()) {
            std::cout << "No notes found.\n";
            return 0;
        }

        std::cout << "Notes:\n";
        std::cout << "======\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            const auto& note = notes[i];
            std::cout << i + 1 << ". [" << note.id << "] " << note.title;
            if (!note.category.empty()) {
                std::cout << " (" << note.category << ")";
            }
            std::cout << "\n";
            if (!note.tags.empty()) {
                std::cout << "   Tags: ";
                for (size_t j = 0; j < note.tags.size(); ++j) {
                    std::cout << note.tags[j];
                    if (j < note.tags.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
        return 0;
    };
    registry_->register_command(note_list_cmd);

    // Get note
    CommandInfo note_get_cmd;
    note_get_cmd.name = "note-get";
    note_get_cmd.description = "View a note";
    note_get_cmd.usage = "note-get <id>";
    note_get_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use notes.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: note-get <id>\n";
            return 1;
        }

        notes::Note note = notes::SnippetManager::instance().get_note(ctx.args[0]);
        if (note.id.empty()) {
            std::cout << "Note not found.\n";
            return 1;
        }

        std::cout << "Title: " << note.title << "\n";
        if (!note.category.empty()) {
            std::cout << "Category: " << note.category << "\n";
        }
        if (!note.tags.empty()) {
            std::cout << "Tags: ";
            for (size_t i = 0; i < note.tags.size(); ++i) {
                std::cout << note.tags[i];
                if (i < note.tags.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }
        std::cout << "Created: " << std::ctime(&note.created);
        std::cout << "Content:\n" << note.content << "\n";
        return 0;
    };
    registry_->register_command(note_get_cmd);

    // Add code snippet
    CommandInfo snippet_add_cmd;
    snippet_add_cmd.name = "snippet-add";
    snippet_add_cmd.description = "Add a code snippet";
    snippet_add_cmd.usage = "snippet-add <title> <language>";
    snippet_add_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use snippets.\n";
            return 1;
        }

        if (ctx.args.size() < 2) {
            std::cout << "Usage: snippet-add <title> <language>\n";
            return 1;
        }

        std::string title = ctx.args[0];
        std::string language = ctx.args[1];

        std::cout << "Enter code snippet (press Enter twice to finish):\n";
        std::string code;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty() && !code.empty()) break;
            if (!code.empty()) code += "\n";
            code += line;
        }

        std::cout << "Description (optional): ";
        std::string description;
        std::getline(std::cin, description);

        std::cout << "Tags (comma-separated, optional): ";
        std::string tags_input;
        std::getline(std::cin, tags_input);

        notes::Tags tags;
        if (!tags_input.empty()) {
            std::stringstream ss(tags_input);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                tag.erase(tag.begin(), std::find_if(tag.begin(), tag.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
                tag.erase(std::find_if(tag.rbegin(), tag.rend(), [](int ch) {
                    return !std::isspace(ch);
                }).base(), tag.end());
                if (!tag.empty()) tags.push_back(tag);
            }
        }

        std::string snippet_id = notes::SnippetManager::instance().add_snippet(title, code, language, description, tags);
        if (!snippet_id.empty()) {
            std::cout << "Snippet added successfully with ID: " << snippet_id << "\n";
            return 0;
        } else {
            std::cout << "Failed to add snippet.\n";
            return 1;
        }
    };
    registry_->register_command(snippet_add_cmd);

    // List snippets
    CommandInfo snippet_list_cmd;
    snippet_list_cmd.name = "snippet-list";
    snippet_list_cmd.description = "List code snippets";
    snippet_list_cmd.usage = "snippet-list [language]";
    snippet_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use snippets.\n";
            return 1;
        }

        std::string language;
        if (!ctx.args.empty()) {
            language = ctx.args[0];
        }

        auto snippets = notes::SnippetManager::instance().list_snippets(language);
        if (snippets.empty()) {
            std::cout << "No snippets found.\n";
            return 0;
        }

        std::cout << "Code Snippets:\n";
        std::cout << "==============\n";
        for (size_t i = 0; i < snippets.size(); ++i) {
            const auto& snippet = snippets[i];
            std::cout << i + 1 << ". [" << snippet.id << "] " << snippet.title 
                     << " (" << snippet.language << ")\n";
            if (!snippet.tags.empty()) {
                std::cout << "   Tags: ";
                for (size_t j = 0; j < snippet.tags.size(); ++j) {
                    std::cout << snippet.tags[j];
                    if (j < snippet.tags.size() - 1) std::cout << ", ";
                }
                std::cout << "\n";
            }
        }
        return 0;
    };
    registry_->register_command(snippet_list_cmd);

    // Get snippet
    CommandInfo snippet_get_cmd;
    snippet_get_cmd.name = "snippet-get";
    snippet_get_cmd.description = "View a code snippet";
    snippet_get_cmd.usage = "snippet-get <id>";
    snippet_get_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use snippets.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: snippet-get <id>\n";
            return 1;
        }

        notes::CodeSnippet snippet = notes::SnippetManager::instance().get_snippet(ctx.args[0]);
        if (snippet.id.empty()) {
            std::cout << "Snippet not found.\n";
            return 1;
        }

        std::cout << "Title: " << snippet.title << "\n";
        std::cout << "Language: " << snippet.language << "\n";
        if (!snippet.description.empty()) {
            std::cout << "Description: " << snippet.description << "\n";
        }
        if (!snippet.tags.empty()) {
            std::cout << "Tags: ";
            for (size_t i = 0; i < snippet.tags.size(); ++i) {
                std::cout << snippet.tags[i];
                if (i < snippet.tags.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }
        std::cout << "Created: " << std::ctime(&snippet.created);
        std::cout << "Code:\n" << snippet.code << "\n";
        return 0;
    };
    registry_->register_command(snippet_get_cmd);

    // Task Scheduler commands
    // Initialize scheduler
    CommandInfo scheduler_init_cmd;
    scheduler_init_cmd.name = "scheduler-init";
    scheduler_init_cmd.description = "Initialize task scheduler";
    scheduler_init_cmd.usage = "scheduler-init";
    scheduler_init_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use task scheduler.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        if (scheduler::TaskScheduler::instance().initialize()) {
            std::cout << "Task scheduler initialized successfully.\n";
            return 0;
        } else {
            std::cout << "Failed to initialize task scheduler.\n";
            return 1;
        }
    };
    registry_->register_command(scheduler_init_cmd);

    // Schedule task
    CommandInfo task_schedule_cmd;
    task_schedule_cmd.name = "task-schedule";
    task_schedule_cmd.description = "Schedule a new task";
    task_schedule_cmd.usage = "task-schedule <title> <command> <minutes>";
    task_schedule_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use task scheduler.\n";
            return 1;
        }

        if (ctx.args.size() < 3) {
            std::cout << "Usage: task-schedule <title> <command> <minutes>\n";
            return 1;
        }

        std::string title = ctx.args[0];
        std::string command = ctx.args[1];
        int minutes;
        try {
            minutes = std::stoi(ctx.args[2]);
        } catch (...) {
            std::cout << "Invalid minutes value.\n";
            return 1;
        }

        time_t scheduled_time = time(nullptr) + (minutes * 60);
        std::string task_id = scheduler::TaskScheduler::instance().schedule_task(
            title, command, scheduled_time, scheduler::RecurrenceType::ONCE);

        if (!task_id.empty()) {
            std::cout << "Task scheduled successfully with ID: " << task_id << "\n";
            return 0;
        } else {
            std::cout << "Failed to schedule task.\n";
            return 1;
        }
    };
    registry_->register_command(task_schedule_cmd);

    // List tasks
    CommandInfo task_list_cmd;
    task_list_cmd.name = "task-list";
    task_list_cmd.description = "List scheduled tasks";
    task_list_cmd.usage = "task-list [category]";
    task_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use task scheduler.\n";
            return 1;
        }

        std::string category;
        if (!ctx.args.empty()) {
            category = ctx.args[0];
        }

        auto tasks = scheduler::TaskScheduler::instance().list_tasks(category);
        if (tasks.empty()) {
            std::cout << "No tasks scheduled.\n";
            return 0;
        }

        std::cout << "Scheduled Tasks:\n";
        std::cout << "================\n";
        for (const auto& task : tasks) {
            std::string status_str;
            switch (task.status) {
                case scheduler::TaskStatus::PENDING: status_str = "Pending"; break;
                case scheduler::TaskStatus::RUNNING: status_str = "Running"; break;
                case scheduler::TaskStatus::COMPLETED: status_str = "Completed"; break;
                case scheduler::TaskStatus::FAILED: status_str = "Failed"; break;
                case scheduler::TaskStatus::CANCELLED: status_str = "Cancelled"; break;
                default: status_str = "Unknown"; break;
            }

            std::cout << "[" << task.id << "] " << task.title << " - " << status_str << "\n";
            std::cout << "  Command: " << task.command << "\n";
            std::cout << "  Scheduled: " << std::ctime(&task.scheduled_time);
        }
        return 0;
    };
    registry_->register_command(task_list_cmd);

    // Cancel task
    CommandInfo task_cancel_cmd;
    task_cancel_cmd.name = "task-cancel";
    task_cancel_cmd.description = "Cancel a scheduled task";
    task_cancel_cmd.usage = "task-cancel <task_id>";
    task_cancel_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use task scheduler.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: task-cancel <task_id>\n";
            return 1;
        }

        bool success = scheduler::TaskScheduler::instance().cancel_task(ctx.args[0]);
        std::cout << (success ? "Task cancelled successfully.\n" : "Failed to cancel task.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(task_cancel_cmd);

    // Run task now
    CommandInfo task_run_cmd;
    task_run_cmd.name = "task-run";
    task_run_cmd.description = "Execute a task immediately";
    task_run_cmd.usage = "task-run <task_id>";
    task_run_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use task scheduler.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: task-run <task_id>\n";
            return 1;
        }

        bool success = scheduler::TaskScheduler::instance().run_task_now(ctx.args[0]);
        std::cout << (success ? "Task executed successfully.\n" : "Failed to execute task.\n");
        return success ? 0 : 1;
    };
    registry_->register_command(task_run_cmd);

    // Add reminder
    CommandInfo remind_add_cmd;
    remind_add_cmd.name = "remind-add";
    remind_add_cmd.description = "Add a reminder";
    remind_add_cmd.usage = "remind-add <title> <minutes>";
    remind_add_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use reminders.\n";
            return 1;
        }

        if (ctx.args.size() < 2) {
            std::cout << "Usage: remind-add <title> <minutes>\n";
            return 1;
        }

        std::string title = ctx.args[0];
        int minutes;
        try {
            minutes = std::stoi(ctx.args[1]);
        } catch (...) {
            std::cout << "Invalid minutes value.\n";
            return 1;
        }

        std::string message = title; // Use title as message for now
        time_t reminder_time = time(nullptr) + (minutes * 60);

        std::string reminder_id = scheduler::TaskScheduler::instance().add_reminder(
            title, message, reminder_time, scheduler::TaskPriority::NORMAL);

        if (!reminder_id.empty()) {
            std::cout << "Reminder set for " << minutes << " minutes from now.\n";
            return 0;
        } else {
            std::cout << "Failed to set reminder.\n";
            return 1;
        }
    };
    registry_->register_command(remind_add_cmd);

    // List reminders
    CommandInfo remind_list_cmd;
    remind_list_cmd.name = "remind-list";
    remind_list_cmd.description = "List active reminders";
    remind_list_cmd.usage = "remind-list";
    remind_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use reminders.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto reminders = scheduler::TaskScheduler::instance().get_active_reminders();
        if (reminders.empty()) {
            std::cout << "No active reminders.\n";
            return 0;
        }

        std::cout << "Active Reminders:\n";
        std::cout << "=================\n";
        for (const auto& reminder : reminders) {
            time_t now = time(nullptr);
            int minutes_left = (reminder.reminder_time - now) / 60;
            std::cout << "[" << reminder.id << "] " << reminder.title << "\n";
            std::cout << "  Time remaining: " << minutes_left << " minutes\n";
        }
        return 0;
    };
    registry_->register_command(remind_list_cmd);

    // Plugin commands
    CommandInfo plugin_list_cmd;
    plugin_list_cmd.name = "plugin-list";
    plugin_list_cmd.description = "List loaded plugins";
    plugin_list_cmd.usage = "plugin-list";
    plugin_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use plugins.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto plugins = plugins::PluginManager::instance().list_plugins();
        if (plugins.empty()) {
            std::cout << "No plugins loaded.\n";
            return 0;
        }

        std::cout << "Loaded Plugins:\n";
        std::cout << "================\n";
        for (size_t i = 0; i < plugins.size(); ++i) {
            const auto& plugin = plugins[i];
            std::cout << i + 1 << ". " << plugin.name << " v" << plugin.version << "\n";
            std::cout << "   " << plugin.description << "\n";
            std::cout << "   Author: " << plugin.author << "\n";
            std::cout << "   Commands: ";
            for (size_t j = 0; j < plugin.commands.size(); ++j) {
                std::cout << plugin.commands[j];
                if (j < plugin.commands.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            if (plugin.loaded) {
                std::cout << "   Status: Loaded\n";
            } else {
                std::cout << "   Status: Unloaded\n";
            }
            std::cout << "\n";
        }
        return 0;
    };
    registry_->register_command(plugin_list_cmd);

    // Scripting commands
    CommandInfo script_run_cmd;
    script_run_cmd.name = "script-run";
    script_run_cmd.description = "Execute a script";
    script_run_cmd.usage = "script-run <script_file>";
    script_run_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use scripting.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: script-run <script_file>\n";
            return 1;
        }

        // Set script context
        scripting::ScriptContext script_ctx;
        script_ctx.current_user = auth::Authentication::instance().get_current_user();
        script_ctx.working_directory = "/"; // Could be improved to use actual working directory
        scripting::ScriptEngine::instance().set_context(script_ctx);

        // Execute the script
        auto result = scripting::ScriptEngine::instance().execute_file(ctx.args[0]);
        
        if (result.success) {
            if (!result.output.empty()) {
                std::cout << result.output;
            }
            return result.exit_code;
        } else {
            std::cout << "Script execution failed: " << result.error << "\n";
            return 1;
        }
    };
    registry_->register_command(script_run_cmd);

    // Logging commands
    CommandInfo log_show_cmd;
    log_show_cmd.name = "log-show";
    log_show_cmd.description = "Show recent log entries";
    log_show_cmd.usage = "log-show [count]";
    log_show_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to view logs.\n";
            return 1;
        }

        size_t count = 10;
        if (!ctx.args.empty()) {
            try {
                count = std::stoul(ctx.args[0]);
            } catch (...) {
                count = 10;
            }
        }

        auto logs = logging::Logger::instance().get_logs(count);
        auto audit = logging::Logger::instance().get_audit_trail(count);

        if (logs.empty() && audit.empty()) {
            std::cout << "No log entries found.\n";
            return 0;
        }

        std::cout << "Recent Log Entries:\n";
        std::cout << "===================\n";

        // Show regular logs
        if (!logs.empty()) {
            std::cout << "\nSystem Logs:\n";
            for (const auto& log : logs) {
                std::string level_str;
                switch (log.level) {
                    case logging::LogLevel::TRACE: level_str = "TRACE"; break;
                    case logging::LogLevel::DEBUG: level_str = "DEBUG"; break;
                    case logging::LogLevel::INFO: level_str = "INFO"; break;
                    case logging::LogLevel::WARNING: level_str = "WARN"; break;
                    case logging::LogLevel::ERROR: level_str = "ERROR"; break;
                    case logging::LogLevel::CRITICAL: level_str = "CRIT"; break;
                }
                std::cout << "[" << level_str << "] " << std::ctime(&log.timestamp)
                         << "  " << log.source << ": " << log.message << "\n";
            }
        }

        // Show audit logs
        if (!audit.empty()) {
            std::cout << "\nAudit Trail:\n";
            for (const auto& entry : audit) {
                std::string event_str;
                switch (entry.event_type) {
                    case logging::AuditEventType::LOGIN: event_str = "LOGIN"; break;
                    case logging::AuditEventType::LOGOUT: event_str = "LOGOUT"; break;
                    case logging::AuditEventType::COMMAND_EXECUTED: event_str = "COMMAND"; break;
                    case logging::AuditEventType::FILE_ACCESSED: event_str = "FILE_ACCESS"; break;
                    case logging::AuditEventType::PERMISSION_CHANGED: event_str = "PERMISSION"; break;
                    case logging::AuditEventType::USER_CREATED: event_str = "USER_CREATE"; break;
                    case logging::AuditEventType::USER_DELETED: event_str = "USER_DELETE"; break;
                    case logging::AuditEventType::VAULT_ACCESSED: event_str = "VAULT_ACCESS"; break;
                    case logging::AuditEventType::NETWORK_ACTIVITY: event_str = "NETWORK"; break;
                    case logging::AuditEventType::PLUGIN_LOADED: event_str = "PLUGIN_LOAD"; break;
                    case logging::AuditEventType::SYSTEM_CONFIG_CHANGED: event_str = "CONFIG_CHANGE"; break;
                }
                std::cout << "[" << event_str << "] " << std::ctime(&entry.timestamp)
                         << "  User: " << entry.user << " - " << entry.action;
                if (!entry.details.empty()) {
                    std::cout << " (" << entry.details << ")";
                }
                std::cout << " [" << (entry.success ? "SUCCESS" : "FAILED") << "]\n";
            }
        }
        return 0;
    };
    registry_->register_command(log_show_cmd);

    // File utilities
    CommandInfo file_list_cmd;
    file_list_cmd.name = "file-list";
    file_list_cmd.description = "List files in directory";
    file_list_cmd.usage = "file-list [directory]";
    file_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use file utilities.\n";
            return 1;
        }

        std::string path = ctx.args.empty() ? "." : ctx.args[0];

        try {
            auto files = vfs::VirtualFileSystem::instance().list(path);
            if (files.empty()) {
                std::cout << "No files found in " << path << "\n";
                return 0;
            }

            std::cout << "Contents of " << path << ":\n";
            std::cout << "========================================\n";
            for (const auto& file : files) {
                std::string type = file.is_directory ? "[DIR]" : "[FILE]";
                std::string size_str;
                if (file.is_directory) {
                    size_str = "<DIR>";
                } else {
                    // Format size
                    if (file.size < 1024) {
                        size_str = std::to_string(file.size) + " B";
                    } else if (file.size < 1024 * 1024) {
                        size_str = std::to_string(file.size / 1024) + " KB";
                    } else if (file.size < 1024 * 1024 * 1024) {
                        size_str = std::to_string(file.size / (1024 * 1024)) + " MB";
                    } else {
                        size_str = std::to_string(file.size / (1024 * 1024 * 1024)) + " GB";
                    }
                }

                std::cout << type << " " << std::left << std::setw(30) << file.name 
                         << std::right << std::setw(10) << size_str << "  "
                         << std::ctime(&file.modified);
            }
            return 0;
        } catch (const std::exception& e) {
            std::cout << "Error listing directory: " << e.what() << "\n";
            return 1;
        }
    };
    registry_->register_command(file_list_cmd);

    // VFS commands
    CommandInfo vfs_mount_cmd;
    vfs_mount_cmd.name = "vfs-mount";
    vfs_mount_cmd.description = "Mount a filesystem";
    vfs_mount_cmd.usage = "vfs-mount <device> <mount_point> [type]";
    vfs_mount_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use VFS.\n";
            return 1;
        }

        if (ctx.args.size() < 2) {
            std::cout << "Usage: vfs-mount <device> <mount_point> [type]\n";
            std::cout << "Types: fat32, ntfs, ext4\n";
            return 1;
        }

        std::string device = ctx.args[0];
        std::string mount_point = ctx.args[1];
        vfs::FSType type = vfs::FSType::UNKNOWN;

        if (ctx.args.size() > 2) {
            std::string type_str = ctx.args[2];
            if (type_str == "fat32") type = vfs::FSType::FAT32;
            else if (type_str == "ntfs") type = vfs::FSType::NTFS;
            else if (type_str == "ext4") type = vfs::FSType::EXT4;
            else {
                std::cout << "Unknown filesystem type: " << type_str << "\n";
                std::cout << "Supported types: fat32, ntfs, ext4\n";
                return 1;
            }
        }

        bool success = vfs::VirtualFileSystem::instance().mount(device, mount_point, type);
        if (success) {
            std::cout << "Filesystem mounted successfully at " << mount_point << "\n";
            return 0;
        } else {
            std::cout << "Failed to mount filesystem.\n";
            return 1;
        }
    };
    registry_->register_command(vfs_mount_cmd);

    // Database commands
    CommandInfo db_connect_cmd;
    db_connect_cmd.name = "db-connect";
    db_connect_cmd.description = "Connect to database";
    db_connect_cmd.usage = "db-connect <type> <name> <connection_details>";
    db_connect_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use database.\n";
            return 1;
        }

        if (ctx.args.size() < 3) {
            std::cout << "Usage: db-connect <type> <name> <connection_details>\n";
            std::cout << "Types: mysql, postgres, sqlite\n";
            std::cout << "Examples:\n";
            std::cout << "  db-connect mysql mydb localhost 3306 testdb user\n";
            std::cout << "  db-connect sqlite mydb /path/to/database.db\n";
            return 1;
        }

        std::string type = ctx.args[0];
        std::string name = ctx.args[1];
        database::DBConfig config;

        bool success = false;
        if (type == "mysql" && ctx.args.size() >= 6) {
            config.type = database::DBType::MYSQL;
            config.host = ctx.args[2];
            config.port = std::stoi(ctx.args[3]);
            config.database = ctx.args[4];
            config.username = ctx.args[5];
            if (ctx.args.size() > 6) config.password = ctx.args[6];
            success = database::DBManager::instance().connect_mysql(name, config.host, config.port,
                                                                  config.database, config.username, config.password);
        } else if (type == "postgres" && ctx.args.size() >= 6) {
            config.type = database::DBType::POSTGRESQL;
            config.host = ctx.args[2];
            config.port = std::stoi(ctx.args[3]);
            config.database = ctx.args[4];
            config.username = ctx.args[5];
            if (ctx.args.size() > 6) config.password = ctx.args[6];
            success = database::DBManager::instance().connect_postgres(name, config.host, config.port,
                                                                     config.database, config.username, config.password);
        } else if (type == "sqlite" && ctx.args.size() >= 3) {
            config.type = database::DBType::SQLITE;
            std::string filepath = ctx.args[2];
            success = database::DBManager::instance().connect_sqlite(name, filepath);
        } else {
            std::cout << "Invalid connection parameters.\n";
            return 1;
        }

        if (success) {
            std::cout << "Connected to " << type << " database '" << name << "' successfully.\n";
            database::DBManager::instance().switch_connection(name);
            return 0;
        } else {
            std::cout << "Failed to connect to database.\n";
            return 1;
        }
    };
    registry_->register_command(db_connect_cmd);

    // P2P commands
    CommandInfo p2p_share_cmd;
    p2p_share_cmd.name = "p2p-share";
    p2p_share_cmd.description = "Share a file via P2P";
    p2p_share_cmd.usage = "p2p-share <file> [--public]";
    p2p_share_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use P2P sharing.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: p2p-share <file> [--public]\n";
            return 1;
        }

        std::string filepath = ctx.args[0];
        bool is_public = false;

        if (ctx.args.size() > 1 && ctx.args[1] == "--public") {
            is_public = true;
        }

        // Ensure P2P server is running
        if (!p2p::FileSharing::instance().is_running()) {
            std::cout << "Starting P2P server...\n";
            if (!p2p::FileSharing::instance().start_server()) {
                std::cout << "Failed to start P2P server.\n";
                return 1;
            }
        }

        std::string file_id = p2p::FileSharing::instance().share_file(filepath, is_public);
        if (!file_id.empty()) {
            std::cout << "File shared successfully!\n";
            std::cout << "File ID: " << file_id << "\n";
            std::cout << "Share type: " << (is_public ? "Public" : "Private") << "\n";
            std::cout << "Other users on the network can now request this file.\n";
            return 0;
        } else {
            std::cout << "Failed to share file.\n";
            return 1;
        }
    };
    registry_->register_command(p2p_share_cmd);

    // AI commands - Temporarily disabled (requires libcurl)
    /*
    CommandInfo ai_suggest_cmd;
    ai_suggest_cmd.name = "ai-suggest";
    ai_suggest_cmd.description = "Get AI-powered command suggestions";
    ai_suggest_cmd.usage = "ai-suggest [context]";
    ai_suggest_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use AI suggestions.\n";
            return 1;
        }

        if (!ai::CommandSuggester::instance().is_initialized()) {
            std::cout << "AI suggestions not initialized. Use 'ai-init <api_key>' first.\n";
            return 1;
        }

        ai::SuggestionContext context;
        context.current_user = auth::Authentication::instance().get_current_user();
        context.current_directory = "/"; // Could be improved to get actual working directory
        context.partial_input = ctx.args.empty() ? "" : ctx.args[0];

        // Get suggestions
        auto suggestions = ai::CommandSuggester::instance().suggest(context);

        if (suggestions.empty()) {
            std::cout << "No AI suggestions available.\n";
            return 0;
        }

        std::cout << "🤖 AI Command Suggestions:\n";
        std::cout << "=========================\n";
        for (size_t i = 0; i < std::min(suggestions.size(), size_t(5)); ++i) {
            const auto& suggestion = suggestions[i];
            std::cout << i + 1 << ". " << suggestion.command << "\n";
            std::cout << "   " << suggestion.description << "\n";
            std::cout << "   Confidence: " << (suggestion.confidence * 100) << "%\n\n";
        }

        // Also show next command predictions
        auto next_commands = ai::CommandSuggester::instance().predict_next_command();
        if (!next_commands.empty()) {
            std::cout << "🎯 Predicted Next Commands:\n";
            std::cout << "===========================\n";
            for (size_t i = 0; i < std::min(next_commands.size(), size_t(3)); ++i) {
                std::cout << i + 1 << ". " << next_commands[i].command << "\n";
            }
        }
        return 0;
    };
    registry_->register_command(ai_suggest_cmd);

    // AI initialization command
    CommandInfo ai_init_cmd;
    ai_init_cmd.name = "ai-init";
    ai_init_cmd.description = "Initialize AI suggestions with API key";
    ai_init_cmd.usage = "ai-init <gemini_api_key>";
    ai_init_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to initialize AI.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: ai-init <gemini_api_key>\n";
            std::cout << "Get your API key from: https://makersuite.google.com/app/apikey\n";
            return 1;
        }

        if (ai::CommandSuggester::instance().initialize(ctx.args[0])) {
            std::cout << "AI suggestions initialized successfully!\n";
            std::cout << "You can now use 'ai-suggest' to get intelligent command suggestions.\n";
            ai::CommandSuggester::instance().enable(true);
            return 0;
        } else {
            std::cout << "Failed to initialize AI suggestions.\n";
            std::cout << "Please check your API key and internet connection.\n";
            return 1;
        }
    };
    registry_->register_command(ai_init_cmd);
    */

    // SSH Remote Shell Access commands
    CommandInfo ssh_start_cmd;
    ssh_start_cmd.name = "ssh-start";
    ssh_start_cmd.description = "Start SSH server for remote access";
    ssh_start_cmd.usage = "ssh-start [port]";
    ssh_start_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to start SSH server.\n";
            return 1;
        }

        uint16_t port = 2222;
        if (!ctx.args.empty()) {
            try {
                port = static_cast<uint16_t>(std::stoi(ctx.args[0]));
            } catch (...) {
                std::cout << "Invalid port number.\n";
                return 1;
            }
        }

        if (remote::SSHServer::instance().start(port)) {
            std::cout << "SSH server started successfully on port " << port << "!\n";
            std::cout << "Remote users can connect with: ssh user@your-ip:" << port << "\n";
            std::cout << "Use 'ssh-stop' to stop the server.\n";
            return 0;
        } else {
            std::cout << "Failed to start SSH server.\n";
            std::cout << "Make sure port " << port << " is not in use.\n";
            return 1;
        }
    };
    registry_->register_command(ssh_start_cmd);

    CommandInfo ssh_stop_cmd;
    ssh_stop_cmd.name = "ssh-stop";
    ssh_stop_cmd.description = "Stop SSH server";
    ssh_stop_cmd.usage = "ssh-stop";
    ssh_stop_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to stop SSH server.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        remote::SSHServer::instance().stop();
        std::cout << "SSH server stopped.\n";
        return 0;
    };
    registry_->register_command(ssh_stop_cmd);

    CommandInfo ssh_connections_cmd;
    ssh_connections_cmd.name = "ssh-connections";
    ssh_connections_cmd.description = "Show active SSH connections";
    ssh_connections_cmd.usage = "ssh-connections";
    ssh_connections_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to view SSH connections.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto connections = remote::SSHServer::instance().get_active_connections();
        if (connections.empty()) {
            std::cout << "No active SSH connections.\n";
            return 0;
        }

        std::cout << "Active SSH Connections:\n";
        std::cout << "======================\n";
        for (const auto& conn : connections) {
            std::cout << "Client: " << conn.client_ip << ":" << conn.client_port << "\n";
            std::cout << "User: " << conn.username << "\n";
            std::cout << "Connected: " << std::ctime(&conn.connected_at);
            std::cout << "Authenticated: " << (conn.authenticated ? "Yes" : "No") << "\n\n";
        }
        return 0;
    };
    registry_->register_command(ssh_connections_cmd);

    // Theme Management commands
    CommandInfo theme_list_cmd;
    theme_list_cmd.name = "theme-list";
    theme_list_cmd.description = "List available themes";
    theme_list_cmd.usage = "theme-list";
    theme_list_cmd.handler = [](const CommandContext& ctx) -> int {
        (void)ctx; // Suppress unused parameter warning
        
        auto themes = novashell::ui::ThemeManager::instance().list_themes();
        if (themes.empty()) {
            std::cout << "No themes available.\n";
            return 1;
        }

        std::cout << "Available Themes:\n";
        std::cout << "=================\n";
        for (size_t i = 0; i < themes.size(); ++i) {
            std::cout << i + 1 << ". " << themes[i] << "\n";
        }

        auto current = novashell::ui::ThemeManager::instance().get_current_theme();
        std::cout << "\nCurrent theme: " << current.name << "\n";
        return 0;
    };
    registry_->register_command(theme_list_cmd);

    CommandInfo theme_set_cmd;
    theme_set_cmd.name = "theme-set";
    theme_set_cmd.description = "Set active theme";
    theme_set_cmd.usage = "theme-set <theme_name>";
    theme_set_cmd.handler = [](const CommandContext& ctx) -> int {
        if (ctx.args.empty()) {
            std::cout << "Usage: theme-set <theme_name>\n";
            std::cout << "Use 'theme-list' to see available themes.\n";
            return 1;
        }

        if (novashell::ui::ThemeManager::instance().load_theme(ctx.args[0])) {
            std::cout << "Theme '" << ctx.args[0] << "' applied successfully!\n";
            return 0;
        } else {
            std::cout << "Failed to apply theme '" << ctx.args[0] << "'.\n";
            return 1;
        }
    };
    registry_->register_command(theme_set_cmd);

    CommandInfo theme_create_cmd;
    theme_create_cmd.name = "theme-create";
    theme_create_cmd.description = "Create a custom theme";
    theme_create_cmd.usage = "theme-create <name>";
    theme_create_cmd.handler = [](const CommandContext& ctx) -> int {
        if (ctx.args.empty()) {
            std::cout << "Usage: theme-create <name>\n";
            return 1;
        }

        std::string name = ctx.args[0];
        
        // Create a basic custom theme
        novashell::ui::ColorScheme colors;
        colors.background = {30, 30, 30};    // Dark background
        colors.foreground = {200, 200, 200}; // Light text
        colors.primary = {100, 150, 255};    // Blue
        colors.secondary = {150, 150, 150}; // Gray
        colors.success = {100, 200, 100};    // Green
        colors.warning = {255, 200, 100};    // Orange
        colors.error = {255, 100, 100};      // Red
        colors.info = {100, 150, 255};       // Blue

        if (novashell::ui::ThemeManager::instance().create_custom_theme(name, colors)) {
            std::cout << "Custom theme '" << name << "' created successfully!\n";
            return 0;
        } else {
            std::cout << "Failed to create theme.\n";
            return 1;
        }
    };
    registry_->register_command(theme_create_cmd);

    // Voice Commands - Temporarily disabled (modules not implemented)
    /*
    CommandInfo voice_start_cmd;
    voice_start_cmd.name = "voice-start";
    voice_start_cmd.description = "Start voice command recognition";
    voice_start_cmd.usage = "voice-start";
    voice_start_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use voice commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        if (!novashell::voice::VoiceCommander::instance().is_initialized()) {
            std::cout << "Voice commands not initialized. Use 'voice-config' first.\n";
            return 1;
        }

        if (novashell::voice::VoiceCommander::instance().start_listening()) {
            std::cout << "🎤 Voice command recognition started!\n";
            std::cout << "Say 'Hey Nova' followed by your command.\n";
            std::cout << "Use 'voice-stop' to stop listening.\n";
            return 0;
        } else {
            std::cout << "Failed to start voice recognition.\n";
            return 1;
        }
    };
    registry_->register_command(voice_start_cmd);

    CommandInfo voice_stop_cmd;
    voice_stop_cmd.name = "voice-stop";
    voice_stop_cmd.description = "Stop voice command recognition";
    voice_stop_cmd.usage = "voice-stop";
    voice_stop_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to use voice commands.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        novashell::voice::VoiceCommander::instance().stop_listening();
        std::cout << "🎤 Voice command recognition stopped.\n";
        return 0;
    };
    registry_->register_command(voice_stop_cmd);

    CommandInfo voice_config_cmd;
    voice_config_cmd.name = "voice-config";
    voice_config_cmd.description = "Configure voice commands";
    voice_config_cmd.usage = "voice-config [api_key]";
    voice_config_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to configure voice commands.\n";
            return 1;
        }

        if (!ctx.args.empty()) {
            // Initialize with API key
            if (novashell::voice::VoiceCommander::instance().initialize(ctx.args[0])) {
                std::cout << "🎤 Voice commands initialized successfully!\n";
                std::cout << "Use 'voice-start' to begin voice recognition.\n";
                return 0;
            } else {
                std::cout << "Failed to initialize voice commands.\n";
                return 1;
            }
        } else {
            // Show current configuration
            auto commands = novashell::voice::VoiceCommander::instance().list_voice_commands();
            std::cout << "Voice Command Configuration:\n";
            std::cout << "============================\n";
            std::cout << "Initialized: " << (novashell::voice::VoiceCommander::instance().is_initialized() ? "Yes" : "No") << "\n";
            std::cout << "Listening: " << (novashell::voice::VoiceCommander::instance().is_listening() ? "Yes" : "No") << "\n";
            std::cout << "Registered Commands: " << commands.size() << "\n";
            return 0;
        }
    };
    registry_->register_command(voice_config_cmd);
    */

    // Analytics Dashboard commands
    CommandInfo dashboard_cmd;
    dashboard_cmd.name = "dashboard";
    dashboard_cmd.description = "Show analytics dashboard";
    dashboard_cmd.usage = "dashboard [name]";
    dashboard_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to view analytics.\n";
            return 1;
        }

        std::string dashboard_name;
        if (!ctx.args.empty()) {
            dashboard_name = ctx.args[0];
        } else {
            // Show main system dashboard
            dashboard_name = "system";
        }

        auto dashboard = novashell::analytics::AnalyticsDashboard::instance().get_dashboard(dashboard_name);
        if (dashboard.name.empty()) {
            std::cout << "Dashboard '" << dashboard_name << "' not found.\n";
            return 1;
        }

        std::cout << "📊 " << dashboard.name << " Dashboard\n";
        std::cout << std::string(dashboard.name.length() + 12, '=') << "\n";

        for (const auto& widget : dashboard.widgets) {
            std::cout << "\n📈 " << widget.title << "\n";
            std::cout << std::string(widget.title.length() + 4, '-') << "\n";

            // Show sample data
            if (!widget.data.data.empty()) {
                for (size_t i = 0; i < std::min(widget.data.data.size(), size_t(5)); ++i) {
                    const auto& point = widget.data.data[i];
                    std::cout << "  " << std::ctime(&point.timestamp)
                             << "  Value: " << point.value << "\n";
                }
            }
        }
        return 0;
    };
    registry_->register_command(dashboard_cmd);

    CommandInfo analytics_cmd;
    analytics_cmd.name = "analytics";
    analytics_cmd.description = "Show analytics metrics";
    analytics_cmd.usage = "analytics [metric]";
    analytics_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to view analytics.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            // Show overview
            auto top_commands = novashell::analytics::AnalyticsDashboard::instance().get_top_commands(5);
            std::cout << "📊 Command Analytics (Top 5):\n";
            std::cout << "=============================\n";
            for (const auto& pair : top_commands) {
                std::cout << "  " << pair.first << ": " << pair.second << " times\n";
            }
            return 0;
        }

        // Show specific metric
        std::string metric = ctx.args[0];
        if (metric == "cpu") {
            auto cpu_history = novashell::analytics::AnalyticsDashboard::instance().get_cpu_usage_history(1);
            std::cout << "📊 CPU Usage History (Last Hour):\n";
            std::cout << "==================================\n";
            for (const auto& point : cpu_history.data) {
                std::cout << "  " << std::ctime(&point.timestamp)
                         << "  CPU: " << point.value << "%\n";
            }
        } else if (metric == "memory") {
            auto mem_history = novashell::analytics::AnalyticsDashboard::instance().get_memory_usage_history(1);
            std::cout << "📊 Memory Usage History (Last Hour):\n";
            std::cout << "=====================================\n";
            for (const auto& point : mem_history.data) {
                std::cout << "  " << std::ctime(&point.timestamp)
                         << "  Memory: " << point.value << "%\n";
            }
        } else {
            std::cout << "Available metrics: cpu, memory\n";
        }
        return 0;
    };
    registry_->register_command(analytics_cmd);

    // Environment Manager commands
    CommandInfo env_switch_cmd;
    env_switch_cmd.name = "env-switch";
    env_switch_cmd.description = "Switch to a different environment profile";
    env_switch_cmd.usage = "env-switch <profile_name>";
    env_switch_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to manage environments.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: env-switch <profile_name>\n";
            std::cout << "Use 'env-list' to see available profiles.\n";
            return 1;
        }

        if (env::EnvironmentManager::instance().switch_profile(ctx.args[0])) {
            std::cout << "🔄 Switched to environment profile: " << ctx.args[0] << "\n";
            
            // Show current environment info
            auto profile = env::EnvironmentManager::instance().get_profile(ctx.args[0]);
            if (!profile.name.empty()) {
                std::cout << "Environment Details:\n";
                if (!profile.sdk_path.empty()) {
                    std::cout << "  SDK Path: " << profile.sdk_path << "\n";
                }
                if (!profile.compiler.empty()) {
                    std::cout << "  Compiler: " << profile.compiler << "\n";
                }
                if (!profile.python_version.empty()) {
                    std::cout << "  Python: " << profile.python_version << "\n";
                }
                if (!profile.node_version.empty()) {
                    std::cout << "  Node.js: " << profile.node_version << "\n";
                }
            }
            return 0;
        } else {
            std::cout << "Failed to switch to profile '" << ctx.args[0] << "'.\n";
            return 1;
        }
    };
    registry_->register_command(env_switch_cmd);

    CommandInfo env_list_cmd;
    env_list_cmd.name = "env-list";
    env_list_cmd.description = "List available environment profiles";
    env_list_cmd.usage = "env-list";
    env_list_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to manage environments.\n";
            return 1;
        }

        (void)ctx; // Suppress unused parameter warning
        auto profiles = env::EnvironmentManager::instance().list_profiles();
        if (profiles.empty()) {
            std::cout << "No environment profiles found.\n";
            std::cout << "Use 'env-create <name>' to create one.\n";
            return 0;
        }

        std::cout << "Environment Profiles:\n";
        std::cout << "=====================\n";
        for (const auto& profile : profiles) {
            std::cout << "📁 " << profile.name << "\n";
            if (!profile.description.empty()) {
                std::cout << "   " << profile.description << "\n";
            }
            if (!profile.sdk_path.empty() || !profile.compiler.empty()) {
                std::cout << "   Tools: ";
                if (!profile.compiler.empty()) std::cout << profile.compiler << " ";
                if (!profile.python_version.empty()) std::cout << "Python " << profile.python_version << " ";
                if (!profile.node_version.empty()) std::cout << "Node " << profile.node_version << " ";
                std::cout << "\n";
            }
            std::cout << "   Last used: " << std::ctime(&profile.last_used);
        }

        auto current = env::EnvironmentManager::instance().get_current_profile();
        if (!current.empty()) {
            std::cout << "\nCurrent profile: " << current << "\n";
        }
        return 0;
    };
    registry_->register_command(env_list_cmd);

    CommandInfo env_create_cmd;
    env_create_cmd.name = "env-create";
    env_create_cmd.description = "Create a new environment profile";
    env_create_cmd.usage = "env-create <name> [description]";
    env_create_cmd.handler = [](const CommandContext& ctx) -> int {
        if (!auth::Authentication::instance().is_logged_in()) {
            std::cout << "You must be logged in to manage environments.\n";
            return 1;
        }

        if (ctx.args.empty()) {
            std::cout << "Usage: env-create <name> [description]\n";
            return 1;
        }

        std::string name = ctx.args[0];
        std::string description;
        if (ctx.args.size() > 1) {
            description = ctx.args[1];
            for (size_t i = 2; i < ctx.args.size(); ++i) {
                description += " " + ctx.args[i];
            }
        }

        if (env::EnvironmentManager::instance().create_profile(name, description)) {
            std::cout << "✅ Environment profile '" << name << "' created successfully!\n";
            return 0;
        } else {
            std::cout << "❌ Failed to create environment profile.\n";
            return 1;
        }
    };
    registry_->register_command(env_create_cmd);
}

// Helper function for showing category help
void CommandProcessor::show_category_help(const std::string& category_name, const std::vector<std::pair<std::string, std::string>>& commands) {
    std::cout << category_name << " Commands:\n";
    std::cout << std::string(category_name.length() + 10, '=') << "\n\n";
    
    for (const auto& cmd : commands) {
        std::cout << cmd.first << "\n";
        std::cout << "  " << cmd.second << "\n\n";
    }
    
    std::cout << "Type 'help <command>' for detailed usage examples.\n";
    std::cout << "📖 See COMMAND_REFERENCE.md for comprehensive examples.\n";
}

} // namespace core
} // namespace customos
