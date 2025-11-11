#include "core/shell.h"
#include "auth/authentication.h"
#include "logging/logger.h"
#include "plugins/plugin_manager.h"
#include "vfs/virtual_filesystem.h"
#include "vault/password_manager.h"
#include "network/packet_analyzer.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

namespace customos {
namespace core {

Shell::Shell() 
    : prompt_("customos> ")
    , running_(false)
    , initialized_(false) {
}

Shell::~Shell() {
    if (initialized_) {
        shutdown();
    }
}

bool Shell::initialize() {
    if (initialized_) {
        return true;
    }

    try {
        // Initialize logger first (using singleton)
        auto& logger = logging::Logger::instance();
        logger.set_log_level(logging::LogLevel::INFO);
        logger.enable_console_output(false);
        logger.enable_file_output(true);
        logger.set_log_file("customos.log");

        LOG_INFO("Initializing CustomOS Shell...");

        // Authentication uses singleton, no need to initialize
        
        // Initialize command processor
        command_processor_ = std::make_unique<CommandProcessor>();
        if (!command_processor_->initialize()) {
            LOG_ERROR("Failed to initialize command processor");
            return false;
        }

        // Load configuration
        load_configuration();

        // Display welcome message
        display_welcome();

        initialized_ = true;
        LOG_INFO("CustomOS Shell initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << "\n";
        return false;
    }
}

void Shell::display_welcome() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          CustomOS Shell - Advanced System Manager         ║\n";
    std::cout << "║                     Version 1.0.0                          ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Type 'help' for available commands or 'exit' to quit.\n";
    std::cout << "\n";
}

void Shell::run() {
    if (!initialized_) {
        std::cerr << "Shell not initialized. Call initialize() first.\n";
        return;
    }

    running_ = true;

    while (running_) {
        try {
            // Display prompt
            std::cout << prompt_;
            std::cout.flush();

            // Read input
            std::string input = read_input();

            // Skip empty lines
            if (input.empty()) {
                continue;
            }

            // Execute command
            if (!execute_command(input)) {
                // Command failed, but continue running
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            LOG_ERROR(std::string("Shell error: ") + e.what());
        }
    }
}

bool Shell::execute_command(const std::string& command) {
    if (command.empty()) {
        return true;
    }

    // Check for built-in shell commands
    if (command == "exit" || command == "quit") {
        running_ = false;
        return true;
    }

    if (command == "clear" || command == "cls") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return true;
    }

    // Log command execution
    logging::Logger::instance().audit_command(command, true);

    // Process command
    auto result = command_processor_->process(command);

    // Display output
    if (!result.output.empty()) {
        std::cout << result.output;
        if (result.output.back() != '\n') {
            std::cout << "\n";
        }
    }

    return result.success;
}

void Shell::shutdown() {
    if (!initialized_) {
        return;
    }

    LOG_INFO("Shutting down CustomOS Shell...");

    running_ = false;
    
    // Save history
    save_history();

    // Cleanup subsystems
    command_processor_.reset();

    initialized_ = false;
}

std::string Shell::get_prompt() const {
    return prompt_;
}

void Shell::set_prompt(const std::string& prompt) {
    prompt_ = prompt;
}

void Shell::load_configuration() {
    // TODO: Load configuration from file
    // For now, use defaults
}

void Shell::save_history() {
    // TODO: Save command history to file
}

std::string Shell::read_input() {
    std::string line;
    std::getline(std::cin, line);
    
    if (std::cin.eof()) {
        running_ = false;
        std::cout << "\n";
        return "";
    }
    
    return line;
}

void Shell::handle_signal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n";
        std::cout << prompt_;
        std::cout.flush();
    }
}

} // namespace core
} // namespace customos
