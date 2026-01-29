#include "ai/ai_module.h"
#include "ai/ai_prompt_manager.h"
#include "database/internal_db.h"
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <regex>
#include <chrono>
#include <iomanip>
#include <fstream>

// For HTTP requests (we'll need to add curl or similar)
#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <curl/curl.h>
#endif

namespace customos {
namespace ai {

// Forward declarations for helper functions
std::string escape_json_string(const std::string& str);
std::vector<std::string> split_string(const std::string& str, char delimiter);

// API Key Manager Implementation
APIKeyManager& APIKeyManager::instance() {
    static APIKeyManager instance;
    return instance;
}

APIKeyManager::APIKeyManager() = default;
APIKeyManager::~APIKeyManager() = default;

bool APIKeyManager::set_api_key(const std::string& api_key) {
    if (!validate_api_key(api_key)) {
        return false;
    }

    try {
        auto& db = database::InternalDB::instance();
        return db.set_config("gemini_api_key", api_key);
    } catch (const std::exception& e) {
        std::cerr << "Failed to store API key: " << e.what() << std::endl;
        return false;
    }
}

std::string APIKeyManager::get_api_key() const {
    try {
        auto& db = database::InternalDB::instance();
        return db.get_config("gemini_api_key", "");
    } catch (const std::exception& e) {
        // API key not found or database error
    }
    return "";
}

bool APIKeyManager::has_api_key() const {
    return !get_api_key().empty();
}

bool APIKeyManager::validate_api_key(const std::string& api_key) {
    // Basic validation - Gemini API keys typically start with "AIza" prefix
    // but we also accept any key that looks reasonable for flexibility
    if (api_key.empty() || api_key.length() < 10) {
        return false;
    }

    // Accept keys starting with AIza (Google API key format)
    if (api_key.find("AIza") == 0) {
        return true;
    }

    // Also accept other reasonable API key formats (alphanumeric with hyphens/underscores)
    for (char c : api_key) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    return api_key.length() >= 20;
}

void APIKeyManager::clear_api_key() {
    try {
        auto& db = database::InternalDB::instance();
        db.delete_config("gemini_api_key");
    } catch (const std::exception& e) {
        std::cerr << "Failed to clear API key: " << e.what() << std::endl;
    }
}

// Gemini Client Implementation
struct GeminiClient::Impl {
    std::string api_key;
    std::string model_name = "gemini-pro";  // Default model, auto-detected based on API key
    std::string api_base = "https://generativelanguage.googleapis.com/v1beta/models/";
    bool initialized = false;

    std::string get_endpoint() const {
        return api_base + model_name + ":generateContent";
    }

    // Auto-detect the best available model based on API key
    void detect_model() {
        // Try gemini-1.5-flash first (faster, more efficient)
        // Fall back to gemini-pro if needed
        // For now, default to gemini-pro which is widely available
        model_name = "gemini-pro";

        // Check if API key supports newer models by examining key pattern
        if (!api_key.empty() && api_key.find("AIza") == 0) {
            // Google AI Studio keys typically support newer models
            model_name = "gemini-1.5-flash";
        }
    }
};

GeminiClient& GeminiClient::instance() {
    static GeminiClient instance;
    return instance;
}

GeminiClient::GeminiClient() : pimpl_(std::make_unique<Impl>()) {}
GeminiClient::~GeminiClient() = default;

bool GeminiClient::initialize(const std::string& api_key) {
    if (!APIKeyManager::instance().validate_api_key(api_key)) {
        return false;
    }

    pimpl_->api_key = api_key;
    pimpl_->detect_model();  // Auto-detect the best model for this API key
    pimpl_->initialized = true;
    std::cout << "Gemini AI initialized with model: " << pimpl_->model_name << std::endl;
    return true;
}

bool GeminiClient::is_initialized() const {
    return pimpl_->initialized;
}

AIResponse GeminiClient::generate_content(const std::string& prompt,
                                        const std::map<std::string, std::string>& options) {
    AIResponse response;
    response.success = false;

    if (!pimpl_->initialized) {
        response.error_message = "Gemini client not initialized";
        return response;
    }

    try {
        // Create JSON payload
        std::stringstream json_payload;
        json_payload << R"(
        {
            "contents": [{
                "parts": [{
                    "text": ")" << escape_json_string(prompt) << R"("
                }]
            }],
            "generationConfig": {
                "temperature": 0.7,
                "topK": 40,
                "topP": 0.95,
                "maxOutputTokens": 1024
            }
        })";

        // Make HTTP request (simplified - would need full implementation)
        std::string url = pimpl_->get_endpoint() + "?key=" + pimpl_->api_key;

        // For now, return a mock response
        // TODO: Implement actual HTTP request using WinHTTP on Windows or libcurl on Linux
        response.success = true;
        response.content = "AI response for: " + prompt.substr(0, 50) + "...";
        response.metadata["model"] = pimpl_->model_name;
        response.metadata["tokens_used"] = "150";
        response.metadata["endpoint"] = url;

    } catch (const std::exception& e) {
        response.error_message = std::string("API call failed: ") + e.what();
    }

    return response;
}

AIResponse GeminiClient::analyze_code(const std::string& code,
                                    const std::string& language,
                                    const std::string& task) {
    std::string prompt = "Analyze this " + language + " code for " + task + ":\n\n" + code;
    return generate_content(prompt);
}

AIResponse GeminiClient::summarize_text(const std::string& text,
                                       const std::string& summary_type) {
    std::string prompt = "Provide a " + summary_type + " summary of the following text:\n\n" + text;
    return generate_content(prompt);
}

// Helper function to escape JSON strings
std::string escape_json_string(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Command Interpreter Implementation
struct CommandInterpreter::Impl {
    std::map<std::string, std::vector<std::string>> learned_mappings;
};

CommandInterpreter& CommandInterpreter::instance() {
    static CommandInterpreter instance;
    return instance;
}

CommandInterpreter::CommandInterpreter() : pimpl_(std::make_unique<Impl>()) {}

std::vector<std::string> CommandInterpreter::interpret_command(const std::string& natural_text) {
    std::vector<std::string> commands;

    // Convert common natural language patterns to shell commands
    std::string lower_text = natural_text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);

    // Pattern matching for common commands
    if (lower_text.find("list files") != std::string::npos ||
        lower_text.find("show files") != std::string::npos) {
        commands.push_back("ls -la");
    }
    else if (lower_text.find("current directory") != std::string::npos ||
             lower_text.find("where am i") != std::string::npos) {
        commands.push_back("pwd");
    }
    else if (lower_text.find("disk usage") != std::string::npos ||
             lower_text.find("disk space") != std::string::npos) {
        commands.push_back("df -h");
    }
    else if (lower_text.find("remind me to") != std::string::npos) {
        // Extract reminder details using regex
        std::regex remind_regex("remind me to (.+?)(?: (every|at|in) (.+?))?$");
        std::smatch matches;
        if (std::regex_search(natural_text, matches, remind_regex)) {
            std::string task = matches[1].str();
            std::string time_spec = matches[3].str();

            // Convert to task-schedule command
            std::string schedule_cmd = "task-schedule \"Reminder: " + task + "\" \"echo 'Reminder: " + task + "'\"";
            if (!time_spec.empty()) {
                // Parse time specification
                if (time_spec.find("daily") != std::string::npos) {
                    schedule_cmd += " --daily 09:00";
                } else if (time_spec.find("hour") != std::string::npos) {
                    schedule_cmd += " --hourly";
                }
            }
            commands.push_back(schedule_cmd);
        }
    }
    else if (lower_text.find("commit") != std::string::npos &&
             lower_text.find("nightly") != std::string::npos) {
        commands.push_back("task-schedule \"Nightly Commit\" \"git add . && git commit -m 'Nightly backup'\" --daily 21:00");
    }

    // If no pattern matched, use AI for interpretation
    if (commands.empty() && GeminiClient::instance().is_initialized()) {
        std::string prompt = "Convert this natural language request to shell commands: '" +
                           natural_text + "'. Return only the commands, one per line.";
        auto ai_response = GeminiClient::instance().generate_content(prompt);
        if (ai_response.success) {
            // Parse AI response into commands
            std::stringstream ss(ai_response.content);
            std::string line;
            while (std::getline(ss, line)) {
                if (!line.empty()) {
                    commands.push_back(line);
                }
            }
        }
    }

    return commands;
}

std::vector<std::string> CommandInterpreter::suggest_commands(const std::string& partial_input) {
    std::vector<std::string> suggestions;

    // Add common completions based on partial input
    std::vector<std::string> common_commands = {
        "help", "exit", "clear", "ls", "pwd", "cd", "mkdir", "rm", "cp", "mv",
        "git status", "git add", "git commit", "git push", "git pull",
        "vault-init", "vault-add", "vault-get", "vault-list",
        "task-schedule", "task-list", "remind", "remind-list"
    };

    for (const auto& cmd : common_commands) {
        if (cmd.find(partial_input) == 0) {
            suggestions.push_back(cmd);
        }
    }

    return suggestions;
}

void CommandInterpreter::learn_correction(const std::string& natural_input,
                                        const std::string& actual_command) {
    pimpl_->learned_mappings[natural_input].push_back(actual_command);
}

// Task Planner Implementation
struct TaskPlanner::Impl {
    std::map<std::string, std::vector<TaskPlanner::TaskStep>> task_templates;
};

TaskPlanner& TaskPlanner::instance() {
    static TaskPlanner instance;
    return instance;
}

TaskPlanner::TaskPlanner() : pimpl_(std::make_unique<Impl>()) {
    // Initialize common task templates
    pimpl_->task_templates["deploy"] = {
        {"git status", "Check repository status", {}, false},
        {"git pull", "Update from remote repository", {}, false},
        {"build-project", "Build the project", {"git pull"}, true},
        {"run-tests", "Execute test suite", {"build-project"}, false},
        {"deploy-app", "Deploy application", {"run-tests"}, true}
    };

    pimpl_->task_templates["backup"] = {
        {"vault-export", "Export vault data", {}, false},
        {"git add .", "Stage all changes", {}, false},
        {"git commit -m 'Backup'", "Commit changes", {"git add ."}, false},
        {"git push", "Push to remote", {"git commit"}, false}
    };
}

std::vector<TaskPlanner::TaskStep> TaskPlanner::plan_task(const std::string& goal) {
    std::vector<TaskStep> plan;

    std::string lower_goal = goal;
    std::transform(lower_goal.begin(), lower_goal.end(), lower_goal.begin(), ::tolower);

    // Check for predefined templates
    if (lower_goal.find("deploy") != std::string::npos) {
        plan = pimpl_->task_templates["deploy"];
    }
    else if (lower_goal.find("backup") != std::string::npos ||
             lower_goal.find("commit") != std::string::npos) {
        plan = pimpl_->task_templates["backup"];
    }
    else if (lower_goal.find("setup workspace") != std::string::npos ||
             lower_goal.find("morning setup") != std::string::npos) {
        plan = {
            {"monitor-start", "Start system monitoring", {}, false},
            {"git status", "Check git status", {}, false},
            {"task-list", "Show today's tasks", {}, false}
        };
    }

    // If no template matched, use AI to generate plan
    if (plan.empty() && GeminiClient::instance().is_initialized()) {
        std::string prompt = "Create a step-by-step plan to accomplish: '" + goal +
                           "'. Return each step as: command|description|dependencies|requires_confirmation";

        auto ai_response = GeminiClient::instance().generate_content(prompt);
        if (ai_response.success) {
            // Parse AI response into task steps
            std::stringstream ss(ai_response.content);
            std::string line;
            while (std::getline(ss, line)) {
                auto parts = split_string(line, '|');
                if (parts.size() >= 3) {
                    TaskStep step;
                    step.command = parts[0];
                    step.description = parts[1];
                    if (parts.size() > 2) {
                        step.dependencies = split_string(parts[2], ',');
                    }
                    step.requires_confirmation = (parts.size() > 3 && parts[3] == "true");
                    plan.push_back(step);
                }
            }
        }
    }

    return plan;
}

bool TaskPlanner::execute_plan(const std::vector<TaskStep>& plan) {
    // This would integrate with the shell's command execution system
    // For now, return true as placeholder
    return true;
}

// Helper function to split strings
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Context Engine Implementation
struct ContextEngine::Impl {
    std::vector<std::string> command_history;
    std::map<std::string, std::map<std::string, std::string>> project_contexts;
    std::string current_project;
};

ContextEngine& ContextEngine::instance() {
    static ContextEngine instance;
    return instance;
}

ContextEngine::ContextEngine() : pimpl_(std::make_unique<Impl>()) {}

void ContextEngine::remember_command(const std::string& command,
                                   const std::string& context) {
    pimpl_->command_history.push_back(command + (context.empty() ? "" : " [" + context + "]"));

    // Keep only last 100 commands
    if (pimpl_->command_history.size() > 100) {
        pimpl_->command_history.erase(pimpl_->command_history.begin());
    }
}

std::vector<std::string> ContextEngine::recall_context(const std::string& query) {
    std::vector<std::string> results;

    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    for (const auto& item : pimpl_->command_history) {
        std::string lower_item = item;
        std::transform(lower_item.begin(), lower_item.end(), lower_item.begin(), ::tolower);

        if (lower_item.find(lower_query) != std::string::npos) {
            results.push_back(item);
        }
    }

    return results;
}

std::string ContextEngine::get_current_context() {
    if (pimpl_->command_history.empty()) {
        return "No recent activity";
    }

    // Return last few commands as context
    std::stringstream ss;
    ss << "Recent activity:\n";
    size_t start = pimpl_->command_history.size() > 5 ?
                   pimpl_->command_history.size() - 5 : 0;

    for (size_t i = start; i < pimpl_->command_history.size(); ++i) {
        ss << "- " << pimpl_->command_history[i] << "\n";
    }

    return ss.str();
}

void ContextEngine::set_project_context(const std::string& project_name,
                                      const std::map<std::string, std::string>& context) {
    pimpl_->project_contexts[project_name] = context;
    pimpl_->current_project = project_name;
}

std::map<std::string, std::string> ContextEngine::get_project_context(const std::string& project_name) {
    auto it = pimpl_->project_contexts.find(project_name);
    if (it != pimpl_->project_contexts.end()) {
        return it->second;
    }
    return {};
}

// AIModule Implementation
struct AIModule::Impl {
    bool initialized = false;
    bool enabled = true;
    AIConfig config;
    std::string stored_api_key;
};

AIModule& AIModule::instance() {
    static AIModule instance;
    return instance;
}

AIModule::AIModule() : pimpl_(std::make_unique<Impl>()) {}
AIModule::~AIModule() = default;

bool AIModule::initialize(const std::string& api_key) {
    // Store and validate API key
    if (!APIKeyManager::instance().validate_api_key(api_key)) {
        std::cerr << "Invalid API key format.\n";
        return false;
    }

    // Store API key
    if (!APIKeyManager::instance().set_api_key(api_key)) {
        std::cerr << "Failed to store API key.\n";
        return false;
    }

    // Initialize Gemini client
    if (!GeminiClient::instance().initialize(api_key)) {
        std::cerr << "Failed to initialize Gemini client.\n";
        return false;
    }

    pimpl_->initialized = true;
    pimpl_->enabled = true;
    pimpl_->stored_api_key = api_key;

    return true;
}

bool AIModule::is_initialized() const {
    return pimpl_->initialized && pimpl_->enabled;
}

bool AIModule::enable() {
    if (pimpl_->stored_api_key.empty()) {
        // Try to load from database
        std::string api_key = APIKeyManager::instance().get_api_key();
        if (api_key.empty()) {
            return false;
        }
        pimpl_->stored_api_key = api_key;
    }

    pimpl_->enabled = true;
    return true;
}

void AIModule::disable() {
    pimpl_->enabled = false;
}

AIConfig AIModule::get_config() const {
    return pimpl_->config;
}

void AIModule::set_config(const AIConfig& config) {
    pimpl_->config = config;
}

std::vector<std::string> AIModule::get_suggestions(const std::string& context) {
    std::vector<std::string> suggestions;

    if (!is_initialized()) {
        return suggestions;
    }

    // Use command interpreter for suggestions
    try {
        suggestions = CommandInterpreter::instance().suggest_commands(context);
    } catch (const std::exception& e) {
        std::cerr << "Error getting suggestions: " << e.what() << std::endl;
    }

    return suggestions;
}

AIResponse AIModule::ask(const std::string& question) {
    AIResponse response;
    response.success = false;

    if (!is_initialized()) {
        response.error_message = "AI module not initialized";
        return response;
    }

    try {
        response = GeminiClient::instance().generate_content(question);
    } catch (const std::exception& e) {
        response.error_message = e.what();
    }

    return response;
}

AIResponse AIModule::analyze(const std::string& code, const std::string& language) {
    AIResponse response;
    response.success = false;

    if (!is_initialized()) {
        response.error_message = "AI module not initialized";
        return response;
    }

    try {
        response = GeminiClient::instance().analyze_code(code, language);
    } catch (const std::exception& e) {
        response.error_message = e.what();
    }

    return response;
}

// Initialize AI modules
bool initialize_ai_modules() {
    // Initialize API key manager
    APIKeyManager::instance();

    // Try to load existing API key
    if (APIKeyManager::instance().has_api_key()) {
        GeminiClient::instance().initialize(APIKeyManager::instance().get_api_key());
    }

    return true;
}

void shutdown_ai_modules() {
    // Cleanup if needed
}

// CodeAnalyzer Implementation
struct CodeAnalyzer::Impl {
    // Implementation details
};

CodeAnalyzer& CodeAnalyzer::instance() {
    static CodeAnalyzer instance;
    return instance;
}

CodeAnalyzer::CodeAnalyzer() : pimpl_(std::make_unique<Impl>()) {}

CodeAnalyzer::CodeAnalysisResult CodeAnalyzer::analyze_file(const std::string& filepath) {
    CodeAnalysisResult result;

    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        result.issues.push_back({"error", "Could not open file: " + filepath, filepath, 0, 0, "", "", 10, "file"});
        return result;
    }

    std::string code((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    // Detect language
    std::string language = "auto";
    if (filepath.find(".") != std::string::npos) {
        std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
        if (ext == "cpp" || ext == "cc" || ext == "cxx") language = "cpp";
        else if (ext == "py") language = "python";
        else if (ext == "js") language = "javascript";
        else if (ext == "java") language = "java";
        else if (ext == "go") language = "go";
        else if (ext == "rs") language = "rust";
    }

    return analyze_code(code, language);
}

CodeAnalyzer::CodeAnalysisResult CodeAnalyzer::analyze_code(const std::string& code, const std::string& language) {
    CodeAnalysisResult result;

    // Basic analysis
    result.total_lines = std::count(code.begin(), code.end(), '\n') + 1;

    // Simple complexity analysis
    result.code_complexity = std::count(code.begin(), code.end(), '{') +
                           std::count(code.begin(), code.end(), '(');

    // Mock analysis results (would be replaced with actual AI analysis)
    result.issues = {
        {"warning", "Consider using const for read-only parameters", "", 15, 10, "int process(int value)", "Add const qualifier", 3, "style"},
        {"info", "Function could be marked as constexpr", "", 25, 5, "int calculate(int x, int y)", "Use constexpr for compile-time evaluation", 2, "optimization"}
    };

    result.issue_counts["warning"] = 1;
    result.issue_counts["info"] = 1;

    result.strengths = {"Good variable naming", "Proper indentation", "Clear function structure"};
    result.recommendations = {"Add error handling", "Consider using smart pointers", "Add unit tests"};

    result.overall_score = "B+"; // Would be calculated by AI

    return result;
}

std::string CodeAnalyzer::generate_code(const CodeGenerationRequest& request) {
    if (!GeminiClient::instance().is_initialized()) {
        return "# Error: AI client not initialized\n# Please run ai-init with your API key";
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = request.language;
    context.task_description = request.description;
    context.parameters = request.parameters;
    context.parameters["type"] = request.type;

    // Add requirements if available
    if (!request.requirements.empty()) {
        std::string reqs;
        for (size_t i = 0; i < request.requirements.size(); ++i) {
            if (i > 0) reqs += "; ";
            reqs += request.requirements[i];
        }
        context.context_info = reqs;
    }

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_code_generation_prompt(context);

    auto response = GeminiClient::instance().generate_content(prompt);
    return response.success ? response.content : "# Failed to generate code";
}

CodeAnalyzer::CodeEditResult CodeAnalyzer::edit_code(const CodeEditRequest& request) {
    CodeEditResult result;
    result.success = false;

    // Read the original file
    std::ifstream file(request.filepath);
    if (!file.is_open()) {
        result.error_message = "Could not open file: " + request.filepath;
        return result;
    }

    std::string original_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    if (!GeminiClient::instance().is_initialized()) {
        result.error_message = "AI client not initialized. Please run ai-init with your API key";
        return result;
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = request.language;
    context.task_description = request.operation;
    context.current_code = original_code;
    context.context_info = request.context;

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_code_editing_prompt(context);

    auto response = GeminiClient::instance().generate_content(prompt);

    if (response.success) {
        result.edited_code = response.content;
        result.success = true;
    } else {
        result.error_message = "Failed to generate edited code";
    }

    return result;
}

CodeAnalyzer::DebugSolution CodeAnalyzer::debug_code(const DebugRequest& request) {
    DebugSolution solution;

    if (!GeminiClient::instance().is_initialized()) {
        solution.explanation = "AI client not initialized";
        solution.confidence = "low";
        return solution;
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = request.language;
    context.error_message = request.error_message;
    context.current_code = request.code_snippet;
    context.context_info = request.context;

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_debugging_prompt(context);

    auto response = GeminiClient::instance().generate_content(prompt);

    if (response.success) {
        // Parse the structured response with advanced parsing
        parse_debug_response(response.content, solution);
    }

    return solution;
}

CodeAnalyzer::CodeExplanation CodeAnalyzer::explain_code(const std::string& code, const std::string& language) {
    CodeExplanation explanation;

    if (!GeminiClient::instance().is_initialized()) {
        explanation.summary = "Error: AI client not initialized. Please run ai-init with your API key";
        return explanation;
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = language;
    context.current_code = code;

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_explanation_prompt(context);

    auto response = GeminiClient::instance().generate_content(prompt);

    if (response.success) {
        // Parse the structured response with advanced parsing
        parse_explanation_response(response.content, explanation);
    } else {
        explanation.summary = "Failed to analyze code";
    }

    return explanation;
}

std::string CodeAnalyzer::generate_documentation(const std::string& code, const std::string& format) {
    if (!GeminiClient::instance().is_initialized()) {
        return "# AI client not initialized";
    }

    std::string prompt = "Generate " + format + " documentation for this code:\n\n" + code;
    auto response = GeminiClient::instance().generate_content(prompt);
    return response.success ? response.content : "# Documentation generation failed";
}

std::string CodeAnalyzer::generate_comments(const std::string& code, const std::string& style) {
    if (!GeminiClient::instance().is_initialized()) {
        return code; // Return original code unchanged
    }

    std::string prompt = "Add " + style + " comments to this code:\n\n" + code;
    auto response = GeminiClient::instance().generate_content(prompt);
    return response.success ? response.content : code;
}

std::string CodeAnalyzer::generate_tests(const TestGenerationRequest& request) {
    if (!GeminiClient::instance().is_initialized()) {
        return "# Error: AI client not initialized\n# Please run ai-init with your API key";
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = request.language;
    context.current_code = request.code;
    context.parameters["framework"] = request.test_framework;

    // Add test types as comma-separated string
    if (!request.test_types.empty()) {
        std::string types_str;
        for (size_t i = 0; i < request.test_types.size(); ++i) {
            if (i > 0) types_str += ",";
            types_str += request.test_types[i];
        }
        context.parameters["test_types"] = types_str;
    }

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_testing_prompt(context);

    auto response = GeminiClient::instance().generate_content(prompt);
    return response.success ? response.content : "# Failed to generate tests";
}

CodeAnalyzer::CodeReviewResult CodeAnalyzer::review_code(const std::string& code, const std::string& language) {
    CodeReviewResult result;

    if (!GeminiClient::instance().is_initialized()) {
        result.overall_rating = "Unable to analyze";
        result.summary = "AI client not initialized";
        return result;
    }

    result.overall_rating = "Good";
    result.critical_issues = {
        {"warning", "Consider adding input validation", "", 10, 5, "function validateInput()", "Add null checks", 4, "security"}
    };
    result.suggestions = {
        {"info", "Consider extracting this into a separate function", "", 25, 1, "long function block", "Improve readability", 2, "maintainability"}
    };
    result.security_concerns = {"Potential SQL injection if input not sanitized"};
    result.best_practices_violated = {"Missing error handling", "No logging"};
    result.metrics = {{"cyclomatic_complexity", 5}, {"maintainability_index", 75}, {"lines_of_code", 150}};
    result.summary = "Code is well-structured with good naming conventions. Consider adding more error handling and input validation.";

    return result;
}

CodeAnalyzer::CodingAssistanceResponse CodeAnalyzer::assist_coding(const CodingAssistanceRequest& request) {
    CodingAssistanceResponse response;

    if (!GeminiClient::instance().is_initialized()) {
        response.answer = "AI client not initialized. Please run ai-init with your API key.";
        return response;
    }

    // Build context for prompt manager
    ai::PromptContext context;
    context.language = request.language;
    context.task_description = request.user_question;
    context.current_code = request.current_code;
    context.context_info = request.context;
    context.skill_level = request.skill_level;

    // Generate optimized prompt using AIPromptManager
    std::string prompt = ai::AIPromptManager::instance().generate_assistance_prompt(context);

    auto ai_response = GeminiClient::instance().generate_content(prompt);

    if (ai_response.success) {
        response.answer = ai_response.content;
        response.suggested_code = "// Suggested code would be here";
        response.next_steps = {"Implement the solution", "Test thoroughly", "Refactor if needed"};
        response.learning_resources = {"Official documentation", "Stack Overflow", "Tutorial websites"};
        response.confidence = "high";
    } else {
        response.answer = "Unable to provide assistance at this time.";
        response.confidence = "low";
    }

    return response;
}

// Advanced response parsing functions
void CodeAnalyzer::parse_debug_response(const std::string& response, DebugSolution& solution) {
    std::string content = response;
    std::transform(content.begin(), content.end(), content.begin(), ::tolower);

    // Parse Root Cause
    size_t root_cause_pos = content.find("root cause:");
    if (root_cause_pos != std::string::npos) {
        size_t start = root_cause_pos + 11; // Length of "root cause:"
        size_t end = content.find("\n", start);
        if (end != std::string::npos) {
            solution.root_cause = response.substr(start, end - start);
            // Clean up whitespace
            solution.root_cause.erase(solution.root_cause.begin(),
                std::find_if(solution.root_cause.begin(), solution.root_cause.end(),
                [](int ch) { return !std::isspace(ch); }));
            solution.root_cause.erase(
                std::find_if(solution.root_cause.rbegin(), solution.root_cause.rend(),
                [](int ch) { return !std::isspace(ch); }).base(), solution.root_cause.end());
        }
    }

    // Parse Fixed Code (look for code blocks)
    size_t code_start = response.find("```");
    if (code_start != std::string::npos) {
        size_t code_end = response.find("```", code_start + 3);
        if (code_end != std::string::npos) {
            solution.fixed_code = response.substr(code_start + 3, code_end - code_start - 3);
            // Remove language specifier if present
            size_t newline_pos = solution.fixed_code.find('\n');
            if (newline_pos != std::string::npos) {
                solution.fixed_code = solution.fixed_code.substr(newline_pos + 1);
            }
        }
    }

    // Parse Alternative Solutions
    size_t alt_start = content.find("alternative solutions:");
    if (alt_start != std::string::npos) {
        solution.alternative_solutions.clear();
        std::string alt_section = response.substr(alt_start);
        std::istringstream iss(alt_section);
        std::string line;

        while (std::getline(iss, line)) {
            // Look for numbered alternatives (1., 2., etc.)
            if (line.find_first_of("123456789") == 0 && line.find('.') != std::string::npos) {
                size_t dot_pos = line.find('.');
                std::string alt = line.substr(dot_pos + 1);
                // Clean up whitespace
                alt.erase(alt.begin(), std::find_if(alt.begin(), alt.end(),
                    [](int ch) { return !std::isspace(ch); }));
                if (!alt.empty()) {
                    solution.alternative_solutions.push_back(alt);
                }
            }
        }
    }

    // Parse Prevention Tips
    size_t prev_start = content.find("prevention tips:");
    if (prev_start != std::string::npos) {
        solution.prevention_tips.clear();
        std::string prev_section = response.substr(prev_start);
        std::istringstream iss(prev_section);
        std::string line;

        while (std::getline(iss, line)) {
            // Look for bullet points
            if (line.find('-') != std::string::npos) {
                size_t dash_pos = line.find('-');
                std::string tip = line.substr(dash_pos + 1);
                // Clean up whitespace
                tip.erase(tip.begin(), std::find_if(tip.begin(), tip.end(),
                    [](int ch) { return !std::isspace(ch); }));
                if (!tip.empty()) {
                    solution.prevention_tips.push_back(tip);
                }
            }
        }
    }

    // Parse Confidence Level
    size_t conf_pos = content.find("confidence:");
    if (conf_pos != std::string::npos) {
        size_t start = conf_pos + 11; // Length of "confidence:"
        size_t end = content.find("\n", start);
        if (end != std::string::npos) {
            std::string conf_level = content.substr(start, end - start);
            if (conf_level.find("high") != std::string::npos) {
                solution.confidence = "high";
            } else if (conf_level.find("medium") != std::string::npos) {
                solution.confidence = "medium";
            } else if (conf_level.find("low") != std::string::npos) {
                solution.confidence = "low";
            } else {
                solution.confidence = "medium";
            }
        }
    } else {
        solution.confidence = "medium";
    }

    // Use the full response as explanation if root cause wasn't found
    if (solution.root_cause.empty()) {
        solution.root_cause = "Analysis completed";
    }

    solution.explanation = response;
}

void CodeAnalyzer::parse_explanation_response(const std::string& response, CodeExplanation& explanation) {
    std::string content = response;
    std::transform(content.begin(), content.end(), content.begin(), ::tolower);

    // Parse Summary
    size_t summary_start = content.find("summary");
    if (summary_start != std::string::npos) {
        size_t colon_pos = response.find(':', summary_start);
        if (colon_pos != std::string::npos) {
            size_t start = colon_pos + 1;
            size_t end = response.find('\n', start);
            if (end != std::string::npos) {
                explanation.summary = response.substr(start, end - start);
                explanation.summary.erase(explanation.summary.begin(),
                    std::find_if(explanation.summary.begin(), explanation.summary.end(),
                    [](int ch) { return !std::isspace(ch); }));
            }
        }
    }

    // Parse Key Concepts
    size_t concepts_start = content.find("key concepts");
    if (concepts_start != std::string::npos) {
        explanation.key_concepts.clear();
        std::string concepts_section = response.substr(concepts_start);
        std::istringstream iss(concepts_section);
        std::string line;

        while (std::getline(iss, line) && explanation.key_concepts.size() < 10) {
            std::string lower_line = line;
            std::transform(lower_line.begin(), lower_line.end(), lower_line.begin(), ::tolower);

            // Skip headers
            if (lower_line.find("key concepts") != std::string::npos ||
                lower_line.find("used:") != std::string::npos) {
                continue;
            }

            // Look for bullet points or list items
            if ((line.find('-') != std::string::npos || line.find('•') != std::string::npos) &&
                line.length() > 3) {
                size_t bullet_pos = line.find('-');
                if (bullet_pos == std::string::npos) {
                    bullet_pos = line.find('•');
                }
                std::string concept = line.substr(bullet_pos + 1);
                concept.erase(concept.begin(), std::find_if(concept.begin(), concept.end(),
                    [](int ch) { return !std::isspace(ch); }));
                if (!concept.empty()) {
                    explanation.key_concepts.push_back(concept);
                }
            }
        }
    }

    // Parse Algorithms Used
    size_t algo_start = content.find("algorithm");
    if (algo_start != std::string::npos) {
        explanation.algorithms_used.clear();
        size_t analysis_pos = content.find("analysis", algo_start);
        if (analysis_pos != std::string::npos) {
            std::string algo_section = response.substr(analysis_pos);
            std::istringstream iss(algo_section);
            std::string line;

            while (std::getline(iss, line) && explanation.algorithms_used.size() < 5) {
                if (line.find('-') != std::string::npos && line.length() > 3) {
                    size_t dash_pos = line.find('-');
                    std::string algo = line.substr(dash_pos + 1);
                    algo.erase(algo.begin(), std::find_if(algo.begin(), algo.end(),
                        [](int ch) { return !std::isspace(ch); }));
                    if (!algo.empty() && algo.find(':') != std::string::npos) {
                        explanation.algorithms_used.push_back(algo);
                    }
                }
            }
        }
    }

    // Parse Function Explanations
    size_t func_start = content.find("function");
    if (func_start != std::string::npos) {
        explanation.function_explanations.clear();
        size_t breakdown_pos = content.find("breakdown", func_start);
        if (breakdown_pos != std::string::npos) {
            std::string func_section = response.substr(breakdown_pos);
            std::istringstream iss(func_section);
            std::string line;
            std::string current_func;

            while (std::getline(iss, line)) {
                if (line.find("**") != std::string::npos && line.find("()") != std::string::npos) {
                    // Extract function name
                    size_t start = line.find("**") + 2;
                    size_t end = line.find("()", start);
                    if (end != std::string::npos) {
                        current_func = line.substr(start, end - start + 2);
                    }
                } else if (!current_func.empty() && !line.empty() &&
                          line.find("**") == std::string::npos) {
                    // Accumulate explanation for current function
                    if (explanation.function_explanations.find(current_func) == explanation.function_explanations.end()) {
                        explanation.function_explanations[current_func] = line;
                    } else {
                        explanation.function_explanations[current_func] += " " + line;
                    }
                }
            }
        }
    }

    // Parse Complexity Analysis
    size_t complex_start = content.find("complexity");
    if (complex_start != std::string::npos) {
        size_t analysis_pos = content.find("analysis", complex_start);
        if (analysis_pos != std::string::npos) {
            size_t start = analysis_pos + 8; // Length of "analysis"
            size_t end = response.find('\n', start);
            if (end != std::string::npos) {
                explanation.complexity_analysis = response.substr(start, end - start);
            }
        }
    }

    // Parse Potential Issues
    size_t issues_start = content.find("potential issues");
    if (issues_start != std::string::npos) {
        explanation.potential_issues.clear();
        std::string issues_section = response.substr(issues_start);
        std::istringstream iss(issues_section);
        std::string line;

        while (std::getline(iss, line) && explanation.potential_issues.size() < 5) {
            if (line.find('-') != std::string::npos && line.length() > 3) {
                size_t dash_pos = line.find('-');
                std::string issue = line.substr(dash_pos + 1);
                issue.erase(issue.begin(), std::find_if(issue.begin(), issue.end(),
                    [](int ch) { return !std::isspace(ch); }));
                if (!issue.empty()) {
                    explanation.potential_issues.push_back(issue);
                }
            }
        }
    }

    // Set defaults if parsing failed
    if (explanation.summary.empty()) {
        explanation.summary = "Code analysis completed - see detailed response";
    }

    if (explanation.key_concepts.empty()) {
        explanation.key_concepts = {"Analysis provided in detailed response"};
    }

    if (explanation.complexity_analysis.empty()) {
        explanation.complexity_analysis = "Complexity analysis included in response";
    }
}

} // namespace ai
} // namespace customos
