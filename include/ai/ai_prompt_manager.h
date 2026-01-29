#ifndef CUSTOMOS_AI_PROMPT_MANAGER_H
#define CUSTOMOS_AI_PROMPT_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>

namespace customos {
namespace ai {

enum class PromptType {
    CODE_GENERATION,
    CODE_EDITING,
    CODE_DEBUGGING,
    CODE_EXPLANATION,
    CODE_TESTING,
    CODING_ASSISTANCE,
    TAB_COMPLETION,
    PRODUCTIVITY_INSIGHTS
};

struct PromptContext {
    std::string language;
    std::string task_description;
    std::string current_code;
    std::string error_message;
    std::string context_info;
    std::unordered_map<std::string, std::string> parameters;
    std::string user;
    std::string current_directory;
    std::string recent_commands;
    int skill_level = 3;
};

class AIPromptManager {
public:
    static AIPromptManager& instance();

    // Generate optimized prompts for different AI tasks
    std::string generate_code_generation_prompt(const PromptContext& context);
    std::string generate_code_editing_prompt(const PromptContext& context);
    std::string generate_debugging_prompt(const PromptContext& context);
    std::string generate_explanation_prompt(const PromptContext& context);
    std::string generate_testing_prompt(const PromptContext& context);
    std::string generate_assistance_prompt(const PromptContext& context);
    std::string generate_tab_completion_prompt(const PromptContext& context);
    std::string generate_insights_prompt(const PromptContext& context);

    // Generic prompt generation
    std::string generate_prompt(PromptType type, const PromptContext& context);

private:
    AIPromptManager();
    ~AIPromptManager();
    AIPromptManager(const AIPromptManager&) = delete;
    AIPromptManager& operator=(const AIPromptManager&) = delete;

    // Prompt building blocks
    std::string build_expert_introduction(PromptType type, const PromptContext& context);
    std::string build_task_description(PromptType type, const PromptContext& context);
    std::string build_requirements_section(PromptType type, const PromptContext& context);
    std::string build_guidelines_section(PromptType type, const PromptContext& context);
    std::string build_output_format_section(PromptType type, const PromptContext& context);
    std::string build_quality_standards_section(PromptType type);

    // Language-specific analysis
    std::string get_language_specific_guidelines(const std::string& language, PromptType type);
    std::string get_framework_specific_guidelines(const std::string& framework);

    // Context building
    std::string build_context_information(const PromptContext& context);
    std::string build_skill_level_guidance(int skill_level);
};

} // namespace ai
} // namespace customos

#endif // CUSTOMOS_AI_PROMPT_MANAGER_H
