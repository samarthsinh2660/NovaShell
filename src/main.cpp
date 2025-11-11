#include <iostream>
#include <cstdlib>
#include <csignal>
#include "core/shell.h"
#include "logging/logger.h"

using namespace customos;

// Global shell instance for signal handling
core::Shell* g_shell = nullptr;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutting down CustomOS Shell...\n";
        if (g_shell) {
            g_shell->shutdown();
        }
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        // Create and initialize the shell
        core::Shell shell;
        g_shell = &shell;

        if (!shell.initialize()) {
            std::cerr << "Failed to initialize CustomOS Shell\n";
            return 1;
        }

        // Check for command-line arguments
        if (argc > 1) {
            // Execute command from arguments
            std::string command;
            for (int i = 1; i < argc; ++i) {
                command += argv[i];
                if (i < argc - 1) command += " ";
            }
            
            bool success = shell.execute_command(command);
            return success ? 0 : 1;
        } else {
            // Run interactive shell
            shell.run();
        }

        // Clean shutdown
        shell.shutdown();
        g_shell = nullptr;

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        logging::Logger::instance().critical(std::string("Fatal error: ") + e.what());
        return 1;
    }
}
