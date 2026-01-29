#ifndef CUSTOMOS_AI_MODULE_H
#define CUSTOMOS_AI_MODULE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace customos {
namespace ai {

// Core AI Response Structure
struct AIResponse {
    std::string content;
    bool success;
    std::string error_message;
    std::map<std::string, std::string> metadata;
};

// API Key Management
class APIKeyManager {
public:
    static APIKeyManager& instance();

    // Store/retrieve API key for current user
    bool set_api_key(const std::string& api_key);
    std::string get_api_key() const;
    bool has_api_key() const;
    bool validate_api_key(const std::string& api_key);

    // Remove API key
    void clear_api_key();

private:
    APIKeyManager();
    ~APIKeyManager();
    APIKeyManager(const APIKeyManager&) = delete;
    APIKeyManager& operator=(const APIKeyManager&) = delete;
};

// Core Gemini API Client
class GeminiClient {
public:
    static GeminiClient& instance();

    bool initialize(const std::string& api_key);
    bool is_initialized() const;

    // Make API calls
    AIResponse generate_content(const std::string& prompt,
                               const std::map<std::string, std::string>& options = {});
    AIResponse analyze_code(const std::string& code,
                           const std::string& language = "auto",
                           const std::string& task = "analyze");
    AIResponse summarize_text(const std::string& text,
                             const std::string& summary_type = "concise");

    // Streaming responses (for future use)
    std::function<void(const std::string&)> on_partial_response;

private:
    GeminiClient();
    ~GeminiClient();
    GeminiClient(const GeminiClient&) = delete;
    GeminiClient& operator=(const GeminiClient&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 1. AI Command Understanding (Natural Language → Actions)
class CommandInterpreter {
public:
    static CommandInterpreter& instance();

    // Convert natural language to shell commands
    std::vector<std::string> interpret_command(const std::string& natural_text);

    // Get command suggestions for partial input
    std::vector<std::string> suggest_commands(const std::string& partial_input);

    // Learn from user corrections
    void learn_correction(const std::string& natural_input,
                         const std::string& actual_command);

private:
    CommandInterpreter();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 2. AI Task Flow Planner (Command Chaining & Automation)
class TaskPlanner {
public:
    static TaskPlanner& instance();

    // Convert goal to multi-step plan
    struct TaskStep {
        std::string command;
        std::string description;
        std::vector<std::string> dependencies;
        bool requires_confirmation;
    };

    std::vector<TaskStep> plan_task(const std::string& goal);

    // Execute planned tasks
    bool execute_plan(const std::vector<TaskStep>& plan);

private:
    TaskPlanner();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 3. AI Memory & Context Engine
class ContextEngine {
public:
    static ContextEngine& instance();

    // Store context
    void remember_command(const std::string& command,
                         const std::string& context = "");

    // Recall context
    std::vector<std::string> recall_context(const std::string& query);

    // Get working context
    std::string get_current_context();

    // Project management
    void set_project_context(const std::string& project_name,
                           const std::map<std::string, std::string>& context);

    std::map<std::string, std::string> get_project_context(const std::string& project_name);

private:
    ContextEngine();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 4. AI Code Analyzer & Helper (Enhanced)
class CodeAnalyzer {
public:
    static CodeAnalyzer& instance();

    // Advanced code analysis
    struct CodeIssue {
        std::string type;        // "bug", "warning", "style", "security", "performance"
        std::string message;
        std::string file;
        int line_number;
        int column;
        std::string code_snippet;
        std::string suggestion;
        std::string fix_code;
        int severity;           // 1-10
        std::string category;
    };

    struct CodeAnalysisResult {
        std::vector<CodeIssue> issues;
        std::map<std::string, int> issue_counts;
        int total_lines;
        int code_complexity;
        std::vector<std::string> strengths;
        std::vector<std::string> recommendations;
        std::string overall_score; // "A", "B", "C", "D", "F"
    };

    // Enhanced analysis methods
    CodeAnalysisResult analyze_file(const std::string& filepath);
    CodeAnalysisResult analyze_code(const std::string& code, const std::string& language = "auto");

    // Code generation
    struct CodeGenerationRequest {
        std::string type;        // "function", "class", "module", "test", "boilerplate"
        std::string language;
        std::string description;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> requirements;
    };

    std::string generate_code(const CodeGenerationRequest& request);

    // Code editing and refactoring
    struct CodeEditRequest {
        std::string filepath;
        std::string operation;   // "refactor", "optimize", "fix", "improve", "document"
        std::string target;      // specific function/class/line
        std::map<std::string, std::string> options;
    };

    struct CodeEditResult {
        bool success;
        std::string edited_code;
        std::vector<std::string> changes_made;
        std::string explanation;
        std::string backup_file; // if file was modified
    };

    CodeEditResult edit_code(const CodeEditRequest& request);

    // Debugging assistance
    struct DebugRequest {
        std::string error_message;
        std::string code_snippet;
        std::string language;
        std::string context;
        std::vector<std::string> stack_trace;
    };

    struct DebugSolution {
        std::string root_cause;
        std::string explanation;
        std::string fixed_code;
        std::vector<std::string> alternative_solutions;
        std::vector<std::string> prevention_tips;
        std::string confidence; // "high", "medium", "low"
    };

    DebugSolution debug_code(const DebugRequest& request);

    // Code explanation and documentation
    struct CodeExplanation {
        std::string summary;
        std::vector<std::string> key_concepts;
        std::vector<std::string> algorithms_used;
        std::map<std::string, std::string> function_explanations;
        std::string complexity_analysis;
        std::vector<std::string> potential_issues;
    };

    CodeExplanation explain_code(const std::string& code, const std::string& language = "auto");

    // Documentation generation
    std::string generate_documentation(const std::string& code, const std::string& format = "markdown");
    std::string generate_comments(const std::string& code, const std::string& style = "detailed");

    // Test generation
    struct TestGenerationRequest {
        std::string code;
        std::string language;
        std::string test_framework;
        std::vector<std::string> test_types; // "unit", "integration", "edge_cases"
    };

    std::string generate_tests(const TestGenerationRequest& request);

    // Code review
    struct CodeReviewResult {
        std::string overall_rating; // "Excellent", "Good", "Needs Work", "Poor"
        std::vector<CodeIssue> critical_issues;
        std::vector<CodeIssue> suggestions;
        std::map<std::string, int> metrics; // maintainability, readability, etc.
        std::vector<std::string> best_practices_violated;
        std::vector<std::string> security_concerns;
        std::string summary;
    };

    CodeReviewResult review_code(const std::string& code, const std::string& language = "auto");

    // Interactive coding assistance
    struct CodingAssistanceRequest {
        std::string current_code;
        std::string user_question;
        std::string language;
        std::string context;
        int skill_level; // 1-5 (beginner to expert)
    };

    struct CodingAssistanceResponse {
        std::string answer;
        std::string suggested_code;
        std::vector<std::string> next_steps;
        std::vector<std::string> learning_resources;
        std::string confidence;
    };

    CodingAssistanceResponse assist_coding(const CodingAssistanceRequest& request);

    // Advanced response parsing functions
    void parse_debug_response(const std::string& response, DebugSolution& solution);
    void parse_explanation_response(const std::string& response, CodeExplanation& explanation);

private:
    CodeAnalyzer();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 5. AI Error & Log Analyzer
class LogAnalyzer {
public:
    static LogAnalyzer& instance();

    // Analyze log files
    struct LogAnalysis {
        std::string issue_type;
        std::string description;
        std::string solution;
        std::vector<std::string> commands_to_fix;
        int confidence;         // 0-100
    };

    std::vector<LogAnalysis> analyze_log(const std::string& log_content);

    // Analyze build errors
    std::vector<LogAnalysis> analyze_build_log(const std::string& build_log);

private:
    LogAnalyzer();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 6. AI Data & File Summarizer
class FileSummarizer {
public:
    static FileSummarizer& instance();

    // Summarize files
    struct FileSummary {
        std::string title;
        std::vector<std::string> key_points;
        std::vector<std::string> action_items;
        std::map<std::string, std::string> metadata;
        std::string summary_type; // "executive", "technical", "concise"
    };

    FileSummary summarize_file(const std::string& filepath,
                              const std::string& summary_type = "concise");

    // Summarize text content
    FileSummary summarize_text(const std::string& content,
                              const std::string& content_type = "text");

private:
    FileSummarizer();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 7. AI Knowledge Graph & Contextual Reasoning
class KnowledgeGraph {
public:
    static KnowledgeGraph& instance();

    // Add knowledge
    void add_fact(const std::string& subject,
                 const std::string& predicate,
                 const std::string& object);

    // Query knowledge
    std::vector<std::string> query(const std::string& question);

    // Connect projects/concepts
    void connect_entities(const std::string& entity1,
                         const std::string& entity2,
                         const std::string& relationship);

    // Get related information
    std::vector<std::string> get_related(const std::string& entity);

private:
    KnowledgeGraph();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 8. AI Routine & Automation Manager
class RoutineManager {
public:
    static RoutineManager& instance();

    // Create routine
    struct Routine {
        std::string name;
        std::string description;
        std::vector<std::string> commands;
        std::string schedule;   // cron format or natural language
        bool enabled;
    };

    bool create_routine(const std::string& name,
                       const std::vector<std::string>& commands,
                       const std::string& description = "");

    // Execute routine
    bool execute_routine(const std::string& name);

    // List routines
    std::vector<Routine> list_routines();

    // Schedule routine
    bool schedule_routine(const std::string& name, const std::string& schedule);

private:
    RoutineManager();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 9. AI Search & Knowledge Recall
class SmartSearch {
public:
    static SmartSearch& instance();

    // Semantic search
    struct SearchResult {
        std::string content;
        std::string type;      // "command", "note", "file", "project"
        std::string source;
        float relevance;       // 0.0 - 1.0
        std::map<std::string, std::string> metadata;
    };

    std::vector<SearchResult> search(const std::string& query,
                                   const std::string& type_filter = "");

    // Index content for search
    void index_command(const std::string& command, const std::string& context = "");
    void index_file(const std::string& filepath, const std::string& content_preview = "");
    void index_note(const std::string& note_id, const std::string& content = "");

private:
    SmartSearch();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// 10. AI Explanation Engine (Educational Layer)
class ExplanationEngine {
public:
    static ExplanationEngine& instance();

    // Explain technical concepts
    std::string explain_concept(const std::string& concept_name,
                              const std::string& context = "");

    // Explain commands
    std::string explain_command(const std::string& command);

    // Generate tutorials
    std::string generate_tutorial(const std::string& topic,
                                const std::string& skill_level = "beginner");

    // Interactive learning
    std::vector<std::string> get_learning_path(const std::string& goal);

private:
    ExplanationEngine();
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// AI Module Configuration
struct AIConfig {
    bool suggestions_enabled = true;
    bool learning_enabled = true;
    bool maintain_context = true;
    size_t max_context_length = 10000;
    std::string preferred_model = "auto";
};

// Unified AI Module Interface
class AIModule {
public:
    static AIModule& instance();

    // Initialize with API key (auto-detects model)
    bool initialize(const std::string& api_key);

    // Check status
    bool is_initialized() const;

    // Enable/Disable
    bool enable();
    void disable();

    // Configuration
    AIConfig get_config() const;
    void set_config(const AIConfig& config);

    // Core AI functions
    std::vector<std::string> get_suggestions(const std::string& context);
    AIResponse ask(const std::string& question);
    AIResponse analyze(const std::string& code, const std::string& language = "auto");

private:
    AIModule();
    ~AIModule();
    AIModule(const AIModule&) = delete;
    AIModule& operator=(const AIModule&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Initialize all AI modules
bool initialize_ai_modules();
void shutdown_ai_modules();

} // namespace ai
} // namespace customos

#endif // CUSTOMOS_AI_MODULE_H
