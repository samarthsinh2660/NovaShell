#include "core/shell.h"
#include "auth/authentication.h"
#include "logging/logger.h"
#include "plugins/plugin_manager.h"
#include "vfs/virtual_filesystem.h"
#include "vault/password_manager.h"
#include "network/packet_analyzer.h"
#include "core/tab_completion.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
#include <conio.h> // For Windows password input
#else
#include <unistd.h>
#include <termios.h>
#endif

namespace customos {
namespace core {

Shell::Shell() 
    : prompt_("novashell> ")
    , running_(false)
    , initialized_(false)
    , history_index_(-1) {
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
        logger.enable_file_output(false);  // Disable file logging

        LOG_INFO("Initializing NovaShell...");

        // Authentication uses singleton, no need to initialize
        
        // Initialize command processor
        command_processor_ = std::make_unique<CommandProcessor>();
        if (!command_processor_->initialize()) {
            LOG_ERROR("Failed to initialize command processor");
            return false;
        }

        // Initialize tab completion
        if (!core::TabCompletion::instance().initialize()) {
            LOG_ERROR("Failed to initialize tab completion");
            return false;
        }

        // Load configuration
        load_configuration();

        // Display welcome message
        display_welcome();

        initialized_ = true;
        LOG_INFO("NovaShell initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << "\n";
        return false;
    }
}

void Shell::display_welcome() {
    std::cout << "\n";
    std::cout << "=======================================================\n";
    std::cout << "                 NovaShell v1.0.0                      \n";
    std::cout << "         Advanced Command Line Interface               \n";
    std::cout << "=======================================================\n";
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

            // Read input with tab completion
            std::string input = read_input_with_completion();

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

    // Add command to history (avoid duplicates of last command)
    if (command_history_.empty() || command_history_.back() != command) {
        command_history_.push_back(command);
        if (command_history_.size() > 1000) {  // Keep last 1000 commands
            command_history_.erase(command_history_.begin());
        }
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

    if (command == "help") {
        show_help();
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

    LOG_INFO("Shutting down NovaShell...");

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

std::string Shell::read_input_with_completion() {
    std::string current_line;
    size_t cursor_pos = 0;
    std::vector<std::string> completion_matches;
    size_t completion_index = 0;
    std::string original_line;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#endif

    while (true) {
        int ch = 0;

#ifdef _WIN32
        DWORD read;
        ReadConsole(hConsole, &ch, 1, &read, NULL);
#else
        ch = getchar();
#endif

        if (ch == '\n' || ch == '\r') {
            std::cout << std::endl;
            break;
        }
        else if (ch == '\t') {  // Tab key pressed
            if (completion_matches.empty()) {
                auto matches = core::TabCompletion::instance().complete(current_line, cursor_pos);
                for (const auto& match : matches) {
                    completion_matches.push_back(match.text);
                }
                completion_index = 0;
            }

            if (!completion_matches.empty()) {
                if (completion_matches.size() == 1) {
                    // Single match - complete immediately
                    std::string completion = completion_matches[0];
                    current_line = completion;
                    cursor_pos = current_line.length();

                    std::cout << "\r" << prompt_;
                    std::cout << current_line;
                    std::cout.flush();
                } else {
                    // Multiple matches - show options below
                    std::cout << "\n";  // Move to next line
                    for (size_t i = 0; i < completion_matches.size() && i < 10; ++i) {
                        if (i > 0) std::cout << "  ";
                        std::cout << completion_matches[i];
                    }
                    std::cout << "\n" << prompt_ << current_line;  // Back to current line
                    std::cout.flush();
                }

                completion_index = (completion_index + 1) % completion_matches.size();
            }
        }
        else if (ch == 27) {  // ESC key - start of escape sequence
            int seq1 = 0, seq2 = 0;

#ifdef _WIN32
            ReadConsole(hConsole, &seq1, 1, &read, NULL);
            if (seq1 == '[') {
                ReadConsole(hConsole, &seq2, 1, &read, NULL);
            }
#else
            seq1 = getchar();
            if (seq1 == '[') {
                seq2 = getchar();
            }
#endif

            if (seq1 == '[') {
                if (seq2 == 'A') {  // Up arrow
                    if (!command_history_.empty()) {
                        if (history_index_ == -1) {
                            original_line = current_line;
                            history_index_ = command_history_.size() - 1;
                        } else if (history_index_ > 0) {
                            history_index_--;
                        }

                        current_line = command_history_[history_index_];
                        cursor_pos = current_line.length();

                        // Clear line and rewrite with prompt
                        std::cout << "\r" << prompt_;
                        std::cout << current_line;
                        std::cout.flush();
                    }
                }
                else if (seq2 == 'B') {  // Down arrow
                    if (history_index_ != -1) {
                        if (history_index_ < static_cast<int>(command_history_.size()) - 1) {
                            history_index_++;
                            current_line = command_history_[history_index_];
                        } else {
                            history_index_ = -1;
                            current_line = original_line;
                        }
                        cursor_pos = current_line.length();

#ifdef _WIN32
                        CONSOLE_SCREEN_BUFFER_INFO csbi;
                        GetConsoleScreenBufferInfo(hOutput, &csbi);
                        COORD startPos = {0, csbi.dwCursorPosition.Y};
                        DWORD written;
                        FillConsoleOutputCharacter(hOutput, ' ', csbi.dwSize.X, startPos, &written);
                        SetConsoleCursorPosition(hOutput, startPos);
                        std::cout << prompt_ << current_line;
#else
                        std::cout << "\r" << prompt_ << current_line;
#endif
                        std::cout.flush();
                    }
                }
            }
            completion_matches.clear();
        }
        else if (ch == 127 || ch == 8) {  // Backspace
            if (!current_line.empty() && cursor_pos > 0) {
                current_line.erase(cursor_pos - 1, 1);
                cursor_pos--;

                // Clear from cursor position to end of line and redraw
#ifdef _WIN32
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(hOutput, &csbi);
                COORD cursorPosCoord = csbi.dwCursorPosition;
                cursorPosCoord.X--;  // Move back one position
                SetConsoleCursorPosition(hOutput, cursorPosCoord);

                // Clear from new cursor position to end of line
                DWORD written;
                FillConsoleOutputCharacter(hOutput, ' ', csbi.dwSize.X - cursorPosCoord.X, cursorPosCoord, &written);

                // Redraw the remaining text
                std::cout << current_line.substr(cursor_pos);
                SetConsoleCursorPosition(hOutput, cursorPosCoord);
#else
                std::cout << "\b\033[K";
                if (cursor_pos < current_line.length()) {
                    std::cout << current_line.substr(cursor_pos);
                    for (size_t i = 0; i < current_line.length() - cursor_pos; ++i) {
                        std::cout << "\b";
                    }
                }
#endif
                std::cout.flush();
            }
            completion_matches.clear();
            history_index_ = -1;
            original_line = current_line;
        }
        else if (ch >= 32 && ch <= 126) {  // Printable character
            current_line.insert(cursor_pos, 1, (char)ch);
            cursor_pos++;

            // Redraw entire line to avoid cursor positioning issues
            std::cout << "\r" << prompt_;
            std::cout << current_line;

            // Position cursor correctly
            if (cursor_pos < current_line.length()) {
                for (size_t i = 0; i < current_line.length() - cursor_pos; ++i) {
                    std::cout << "\b";
                }
            }
            std::cout.flush();

            completion_matches.clear();
            history_index_ = -1;
            original_line = current_line;
        }
        // Ignore other control characters
    }

#ifdef _WIN32
    SetConsoleMode(hConsole, mode);
#endif

    return current_line;
}

void Shell::handle_signal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n";
        std::cout << prompt_;
        std::cout.flush();
    }
}

void Shell::show_help() {
    std::cout << "NovaShell Help" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Commands:" << std::endl;
    std::cout << "  help     - Show this help message" << std::endl;
    std::cout << "  version  - Show version information" << std::endl;
    std::cout << "  echo     - Display text" << std::endl;
    std::cout << "  whoami   - Show current user" << std::endl;
    std::cout << "  login    - Login as a user" << std::endl;
    std::cout << "  exit     - Exit the shell" << std::endl;
    std::cout << "  quit     - Exit the shell" << std::endl;
    std::cout << "  clear    - Clear the screen" << std::endl;
    std::cout << "  cls      - Clear the screen (Windows)" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Modules:" << std::endl;
    std::cout << "  git      - Git version control commands" << std::endl;
    std::cout << "  db       - Database operations" << std::endl;
    std::cout << "  vault    - Password management" << std::endl;
    std::cout << "  net      - Network analysis tools" << std::endl;
    std::cout << "  monitor  - System monitoring" << std::endl;
    std::cout << "  ai       - AI-powered suggestions" << std::endl;
    std::cout << "  note     - Note and snippet management" << std::endl;
    std::cout << "  env      - Environment management" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on any command, type: help <command>" << std::endl;
    std::cout << std::endl;
}

} // namespace core
} // namespace customos
