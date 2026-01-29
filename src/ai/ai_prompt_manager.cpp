#include "ai/ai_prompt_manager.h"
#include <sstream>
#include <iomanip>

namespace customos {
namespace ai {

AIPromptManager& AIPromptManager::instance() {
    static AIPromptManager instance;
    return instance;
}

AIPromptManager::AIPromptManager() {
}

AIPromptManager::~AIPromptManager() {
}

std::string AIPromptManager::generate_prompt(PromptType type, const PromptContext& context) {
    switch (type) {
        case PromptType::CODE_GENERATION:
            return generate_code_generation_prompt(context);
        case PromptType::CODE_EDITING:
            return generate_code_editing_prompt(context);
        case PromptType::CODE_DEBUGGING:
            return generate_debugging_prompt(context);
        case PromptType::CODE_EXPLANATION:
            return generate_explanation_prompt(context);
        case PromptType::CODE_TESTING:
            return generate_testing_prompt(context);
        case PromptType::CODING_ASSISTANCE:
            return generate_assistance_prompt(context);
        case PromptType::TAB_COMPLETION:
            return generate_tab_completion_prompt(context);
        case PromptType::PRODUCTIVITY_INSIGHTS:
            return generate_insights_prompt(context);
        default:
            return "";
    }
}

std::string AIPromptManager::generate_code_generation_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODE_GENERATION, context);
    prompt << build_task_description(PromptType::CODE_GENERATION, context);
    prompt << build_context_information(context);
    prompt << build_requirements_section(PromptType::CODE_GENERATION, context);
    prompt << build_guidelines_section(PromptType::CODE_GENERATION, context);
    prompt << build_output_format_section(PromptType::CODE_GENERATION, context);
    prompt << build_quality_standards_section(PromptType::CODE_GENERATION);

    if (context.parameters.count("type")) {
        if (context.parameters.at("type") == "function") {
            prompt << "\nFUNCTION SPECIFICS:\n";
            prompt << "- Include function signature with appropriate parameters\n";
            prompt << "- Add parameter validation\n";
            prompt << "- Return appropriate values or handle errors\n";
            prompt << "- Include time/space complexity comments\n\n";
        } else if (context.parameters.at("type") == "class") {
            prompt << "\nCLASS SPECIFICS:\n";
            prompt << "- Include constructor(s) and destructor if needed\n";
            prompt << "- Implement proper encapsulation\n";
            prompt << "- Add getter/setter methods if appropriate\n";
            prompt << "- Include class documentation\n\n";
        } else if (context.parameters.at("type") == "test") {
            prompt << "\nTEST SPECIFICS:\n";
            prompt << "- Cover normal cases, edge cases, and error conditions\n";
            prompt << "- Use appropriate testing framework conventions\n";
            prompt << "- Include setup and teardown if needed\n";
            prompt << "- Add descriptive test names\n\n";
        }
    }

    prompt << "Generate the " << context.language << " code now:";

    return prompt.str();
}

std::string AIPromptManager::generate_code_editing_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODE_EDITING, context);
    prompt << build_task_description(PromptType::CODE_EDITING, context);
    prompt << build_context_information(context);

    if (!context.current_code.empty()) {
        prompt << "\nORIGINAL CODE:\n";
        prompt << "```\n" << context.current_code << "\n```\n\n";
    }

    prompt << build_guidelines_section(PromptType::CODE_EDITING, context);
    prompt << build_output_format_section(PromptType::CODE_EDITING, context);
    prompt << build_quality_standards_section(PromptType::CODE_EDITING);

    prompt << "Generate the refactored code now:";

    return prompt.str();
}

std::string AIPromptManager::generate_debugging_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODE_DEBUGGING, context);
    prompt << build_task_description(PromptType::CODE_DEBUGGING, context);

    prompt << "\nERROR INFORMATION:\n";
    prompt << "- Error Message: " << context.error_message << "\n";
    prompt << "- Programming Language: " << context.language << "\n";

    if (!context.context_info.empty()) {
        prompt << "- Context: " << context.context_info << "\n";
    }

    if (!context.current_code.empty()) {
        prompt << "\nCODE SNIPPET:\n```" << context.language << "\n" << context.current_code << "\n```\n";
    }

    prompt << "\nANALYSIS REQUIREMENTS:\n";
    prompt << "1. **Root Cause Identification**: Determine the exact cause of the error\n";
    prompt << "2. **Impact Assessment**: Explain what the error affects and why it occurs\n";
    prompt << "3. **Code Fix**: Provide corrected code with explanations\n";
    prompt << "4. **Alternative Solutions**: Suggest 2-3 different approaches\n";
    prompt << "5. **Prevention Tips**: How to avoid this type of error in the future\n";
    prompt << "6. **Confidence Level**: Rate certainty of the analysis (high/medium/low)\n\n";

    prompt << "DEBUGGING METHODOLOGY:\n";
    prompt << "- Check for common " << context.language << " error patterns\n";
    prompt << "- Analyze variable initialization and scoping\n";
    prompt << "- Review memory management and resource handling\n";
    prompt << "- Examine control flow and logic errors\n";
    prompt << "- Consider race conditions and threading issues\n";
    prompt << "- Validate input handling and boundary conditions\n\n";

    prompt << build_output_format_section(PromptType::CODE_DEBUGGING, context);
    prompt << build_quality_standards_section(PromptType::CODE_DEBUGGING);

    prompt << "Now analyze the error and provide your expert debugging solution:";

    return prompt.str();
}

std::string AIPromptManager::generate_explanation_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODE_EXPLANATION, context);
    prompt << build_task_description(PromptType::CODE_EXPLANATION, context);

    if (!context.current_code.empty()) {
        prompt << "\nCODE TO ANALYZE:\n";
        prompt << "```\n" << context.current_code << "\n```\n\n";
    }

    prompt << "ANALYSIS REQUIREMENTS:\n";
    prompt << "1. **Summary**: What does this code do? (1-2 sentences)\n";
    prompt << "2. **Key Concepts**: What programming concepts are demonstrated?\n";
    prompt << "3. **Algorithms & Patterns**: What algorithms, design patterns, or techniques are used?\n";
    prompt << "4. **Function Analysis**: Detailed breakdown of each function/method\n";
    prompt << "5. **Complexity Analysis**: Time/space complexity, performance characteristics\n";
    prompt << "6. **Potential Issues**: Bugs, security concerns, or improvement opportunities\n\n";

    prompt << "DETAILED ANALYSIS FRAMEWORK:\n";
    prompt << "- **Purpose & Context**: Why was this code written? What problem does it solve?\n";
    prompt << "- **Architecture**: How is the code structured? What are the main components?\n";
    prompt << "- **Data Flow**: How does data move through the system?\n";
    prompt << "- **Control Flow**: What is the execution path and decision logic?\n";
    prompt << "- **Error Handling**: How are errors detected and handled?\n";
    prompt << "- **Resource Management**: Memory, files, network resources usage\n";
    prompt << "- **Threading/Concurrency**: If applicable, how are concurrent operations handled?\n\n";

    prompt << get_language_specific_guidelines(context.language, PromptType::CODE_EXPLANATION);
    prompt << build_output_format_section(PromptType::CODE_EXPLANATION, context);
    prompt << build_quality_standards_section(PromptType::CODE_EXPLANATION);

    prompt << "Begin your comprehensive code analysis now:";

    return prompt.str();
}

std::string AIPromptManager::generate_testing_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODE_TESTING, context);
    prompt << build_task_description(PromptType::CODE_TESTING, context);

    if (!context.current_code.empty()) {
        prompt << "\nSOURCE CODE TO TEST:\n";
        prompt << "```\n" << context.current_code << "\n```\n\n";
    }

    prompt << "TEST REQUIREMENTS:\n";
    prompt << "- Framework: " << context.parameters.at("framework") << "\n";
    prompt << "- Language: " << context.language << "\n";

    if (context.parameters.count("test_types")) {
        prompt << "- Test Types: " << context.parameters.at("test_types") << "\n";
    }
    prompt << "\n";

    prompt << build_guidelines_section(PromptType::CODE_TESTING, context);
    prompt << build_output_format_section(PromptType::CODE_TESTING, context);
    prompt << build_quality_standards_section(PromptType::CODE_TESTING);

    prompt << "Generate the comprehensive test suite now:";

    return prompt.str();
}

std::string AIPromptManager::generate_assistance_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << build_expert_introduction(PromptType::CODING_ASSISTANCE, context);
    prompt << build_task_description(PromptType::CODING_ASSISTANCE, context);

    prompt << "\nSTUDENT INFORMATION:\n";
    prompt << "- Skill Level: " << build_skill_level_guidance(context.skill_level) << "\n";
    prompt << "- Programming Language: " << context.language << "\n";
    prompt << "- Question: " << context.task_description << "\n";

    if (!context.current_code.empty()) {
        prompt << "\nCURRENT CODE CONTEXT:\n```" << context.language << "\n" << context.current_code << "\n```\n";
    }

    if (!context.context_info.empty()) {
        prompt << "\nADDITIONAL CONTEXT:\n" << context.context_info << "\n";
    }

    prompt << "\nEDUCATIONAL APPROACH:\n";
    prompt << build_skill_level_guidance(context.skill_level);

    prompt << "\nRESPONSE STRUCTURE:\n";
    prompt << "1. **Understanding Check**: Confirm understanding of the question\n";
    prompt << "2. **Conceptual Explanation**: Explain relevant concepts clearly\n";
    prompt << "3. **Practical Solution**: Provide working code examples\n";
    prompt << "4. **Step-by-Step Guidance**: Break down complex solutions\n";
    prompt << "5. **Best Practices**: Highlight important conventions and patterns\n";
    prompt << "6. **Common Pitfalls**: Warn about frequent mistakes\n";
    prompt << "7. **Next Steps**: Suggest what to learn or try next\n";
    prompt << "8. **Additional Resources**: Recommend learning materials\n\n";

    prompt << "CODE EXAMPLE REQUIREMENTS:\n";
    prompt << "- Include complete, runnable code snippets\n";
    prompt << "- Add detailed comments explaining each part\n";
    prompt << "- Show both correct and incorrect approaches (with explanations)\n";
    prompt << "- Include error handling and edge cases\n";
    prompt << "- Follow language-specific best practices\n\n";

    prompt << "TEACHING PHILOSOPHY:\n";
    prompt << "- **Patient and Encouraging**: Support learning at all levels\n";
    prompt << "- **Practical Focus**: Emphasize real-world applicability\n";
    prompt << "- **Progressive Learning**: Build understanding step by step\n";
    prompt << "- **Error-Friendly**: Help learn from mistakes\n";
    prompt << "- **Context-Aware**: Consider the student's background and goals\n\n";

    prompt << build_quality_standards_section(PromptType::CODING_ASSISTANCE);

    prompt << "Now provide your expert coding assistance:";

    return prompt.str();
}

std::string AIPromptManager::generate_tab_completion_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << "You are an expert command-line completion AI for NovaShell, a powerful terminal assistant.\n\n";

    prompt << build_context_information(context);

    prompt << "CURRENT COMMAND BEING TYPED:\n";
    prompt << "\"" << context.task_description << "\"\n\n";

    prompt << "COMMAND COMPLETION TASK:\n";
    prompt << "Analyze the partial command above and suggest the 3-5 most likely completions.\n";
    prompt << "Consider the context, recent usage patterns, and typical command-line workflows.\n\n";

    prompt << "COMPLETION STRATEGIES TO USE:\n";
    prompt << "1. **Common Command Patterns**: Complete frequently used command combinations\n";
    prompt << "2. **Context-Aware Completion**: Consider current directory and project type\n";
    prompt << "3. **Workflow Sequences**: Suggest logical next commands in development workflows\n";
    prompt << "4. **Time-Based Suggestions**: Commands commonly used at current time of day\n";
    prompt << "5. **Project-Specific**: Commands appropriate for the detected project type\n\n";

    prompt << "OUTPUT FORMAT:\n";
    prompt << "Return ONLY the command completions, one per line.\n";
    prompt << "Each completion should be a valid, complete command that would logically follow from the partial input.\n";
    prompt << "Focus on practical, commonly used commands in development environments.\n";
    prompt << "Prioritize commands that are likely to be useful given the current context.\n\n";

    prompt << "EXAMPLES OF GOOD COMPLETIONS:\n";
    prompt << "For \"git \": git status, git add ., git commit -m \"update\", git push origin main\n";
    prompt << "For \"ai-\": ai-analyze main.cpp, ai-generate function cpp, ai-help \"how to\"\n";
    prompt << "For \"docker \": docker ps, docker build ., docker run -it ubuntu\n\n";

    prompt << "Now provide the most relevant command completions:";

    return prompt.str();
}

std::string AIPromptManager::generate_insights_prompt(const PromptContext& context) {
    std::ostringstream prompt;

    prompt << "You are an AI productivity analyst for NovaShell.\n\n";
    prompt << "Based on the following productivity metrics, generate one key insight:\n";
    prompt << "User: " << context.user << "\n";

    if (context.parameters.count("productivity_score")) {
        prompt << "Productivity Score: " << context.parameters.at("productivity_score") << "\n";
    }
    if (context.parameters.count("ai_adoption_rate")) {
        prompt << "AI Adoption Rate: " << context.parameters.at("ai_adoption_rate") << "\n";
    }

    return prompt.str();
}

// Helper methods implementation
std::string AIPromptManager::build_expert_introduction(PromptType type, const PromptContext& context) {
    std::ostringstream intro;

    switch (type) {
        case PromptType::CODE_GENERATION:
            intro << "You are an expert " << context.language << " developer and code generation AI for NovaShell.\n\n";
            break;
        case PromptType::CODE_EDITING:
            intro << "You are an expert code refactoring AI for NovaShell, specializing in " << context.task_description << " operations.\n\n";
            break;
        case PromptType::CODE_DEBUGGING:
            intro << "You are an expert debugging AI for NovaShell, specializing in " << context.language << " error analysis and resolution.\n\n";
            break;
        case PromptType::CODE_EXPLANATION:
            intro << "You are an expert " << context.language << " code analyst for NovaShell, providing detailed technical explanations.\n\n";
            break;
        case PromptType::CODE_TESTING:
            intro << "You are an expert test engineer for NovaShell, specializing in " << context.language << " test generation using " << context.parameters.at("framework") << ".\n\n";
            break;
        case PromptType::CODING_ASSISTANCE:
            intro << "You are an expert " << context.language << " programming tutor and coding assistant for NovaShell.\n\n";
            break;
        default:
            intro << "You are an AI assistant for NovaShell.\n\n";
            break;
    }

    return intro.str();
}

std::string AIPromptManager::build_task_description(PromptType type, const PromptContext& context) {
    std::ostringstream desc;

    switch (type) {
        case PromptType::CODE_GENERATION:
            desc << "CODE GENERATION TASK:\n";
            desc << "Generate high-quality, production-ready " << context.language << " code for: " << context.task_description << "\n\n";
            break;
        case PromptType::CODE_EDITING:
            desc << "REFACTORING TASK:\n";
            desc << "Apply " << context.task_description << " refactoring to the following code.\n\n";
            break;
        case PromptType::CODE_DEBUGGING:
            desc << "DEBUGGING TASK:\n";
            desc << "Analyze the following error and provide a comprehensive solution.\n\n";
            break;
        case PromptType::CODE_EXPLANATION:
            desc << "CODE ANALYSIS TASK:\n";
            desc << "Provide a comprehensive explanation of the following " << context.language << " code.\n\n";
            break;
        case PromptType::CODE_TESTING:
            desc << "TEST GENERATION TASK:\n";
            desc << "Generate comprehensive, high-quality test cases for the following " << context.language << " code.\n\n";
            break;
        case PromptType::CODING_ASSISTANCE:
            desc << "CODING ASSISTANCE REQUEST:\n";
            desc << "Provide educational, helpful guidance for a " << build_skill_level_guidance(context.skill_level) << " level developer.\n\n";
            break;
        default:
            desc << "TASK: " << context.task_description << "\n\n";
            break;
    }

    return desc.str();
}

std::string AIPromptManager::build_context_information(const PromptContext& context) {
    std::ostringstream ctx;

    ctx << "CONTEXT INFORMATION:\n";
    if (!context.current_directory.empty()) {
        ctx << "- Current directory: " << context.current_directory << "\n";
    }
    if (!context.user.empty()) {
        ctx << "- User: " << context.user << "\n";
    }
    ctx << "- NovaShell is a comprehensive terminal with AI features, Git integration, database tools, and more\n";

    if (!context.recent_commands.empty()) {
        ctx << "\nRecent commands used:\n" << context.recent_commands << "\n";
    }
    ctx << "\n";

    return ctx.str();
}

std::string AIPromptManager::build_requirements_section(PromptType type, const PromptContext& context) {
    std::ostringstream req;

    if (type == PromptType::CODE_GENERATION) {
        req << "REQUIREMENTS:\n";
        req << "- Type: " << context.parameters.at("type") << "\n";
        req << "- Language: " << context.language << "\n";
        req << "- Follow " << context.language << " best practices and conventions\n";
        req << "- Include proper error handling and validation\n";
        req << "- Add comprehensive comments and documentation\n";
        req << "- Make code modular and reusable\n";
        req << "- Include example usage\n\n";
    }

    return req.str();
}

std::string AIPromptManager::build_guidelines_section(PromptType type, const PromptContext& context) {
    std::ostringstream guide;

    switch (type) {
        case PromptType::CODE_EDITING:
            guide << "REFACTORING GUIDELINES:\n";
            guide << get_language_specific_guidelines(context.language, type);
            break;
        case PromptType::CODE_TESTING:
            guide << "TEST COVERAGE REQUIREMENTS:\n";
            guide << "1. **Unit Tests**: Test individual functions/methods in isolation\n";
            guide << "2. **Integration Tests**: Test component interactions and data flow\n";
            guide << "3. **Edge Cases**: Boundary conditions, error inputs, unusual scenarios\n";
            guide << "4. **Performance Tests**: If applicable, test performance characteristics\n\n";

            guide << "TEST DESIGN PRINCIPLES:\n";
            guide << "- **Arrange-Act-Assert (AAA)**: Clear test structure\n";
            guide << "- **Descriptive Names**: Tests should explain what they verify\n";
            guide << "- **Independent Tests**: Each test should run independently\n";
            guide << "- **Maintainable**: Easy to understand and modify\n";
            guide << "- **Comprehensive**: Cover happy path, error paths, and edge cases\n\n";

            guide << "FRAMEWORK-SPECIFIC GUIDELINES:\n";
            guide << get_framework_specific_guidelines(context.parameters.at("framework"));
            break;
        default:
            break;
    }

    return guide.str();
}

std::string AIPromptManager::build_output_format_section(PromptType type, const PromptContext& context) {
    std::ostringstream format;

    switch (type) {
        case PromptType::CODE_GENERATION:
        case PromptType::CODE_EDITING:
            format << "OUTPUT REQUIREMENTS:\n";
            format << "- Return ONLY the complete, runnable code\n";
            format << "- Include all necessary imports/includes at the top\n";
            format << "- Add inline comments explaining complex logic\n";
            format << "- Ensure code compiles and runs correctly\n";
            format << "- Maintain the same public interface\n\n";
            break;
        case PromptType::CODE_DEBUGGING:
            format << "OUTPUT FORMAT:\n";
            format << "Root Cause: [Brief, clear explanation]\n\n";
            format << "Explanation: [Detailed technical explanation of why the error occurs]\n\n";
            format << "Fixed Code:\n```" << context.language << "\n[Complete corrected code]\n```\n\n";
            format << "Alternative Solutions:\n";
            format << "1. [First alternative approach]\n";
            format << "2. [Second alternative approach]\n";
            format << "3. [Third alternative approach, if applicable]\n\n";
            format << "Prevention Tips:\n";
            format << "- [Specific tip 1]\n";
            format << "- [Specific tip 2]\n";
            format << "- [Specific tip 3]\n\n";
            format << "Confidence: [high/medium/low] (explain reasoning)\n\n";
            break;
        case PromptType::CODE_TESTING:
            format << "OUTPUT REQUIREMENTS:\n";
            format << "- Return ONLY the complete test code\n";
            format << "- Include all necessary imports and setup\n";
            format << "- Add comprehensive test documentation\n";
            format << "- Ensure tests are runnable and follow framework conventions\n";
            format << "- Include setup and teardown where appropriate\n\n";
            break;
        case PromptType::CODE_EXPLANATION:
            format << "OUTPUT STRUCTURE:\n";
            format << "1. **Summary**\n";
            format << "   [Brief overview of what the code accomplishes]\n\n";
            format << "2. **Key Concepts Used**\n";
            format << "   - [Concept 1]: [Explanation]\n";
            format << "   - [Concept 2]: [Explanation]\n";
            format << "   - [Concept N]: [Explanation]\n\n";
            format << "3. **Algorithm/Pattern Analysis**\n";
            format << "   - [Algorithm/Pattern]: [Detailed explanation]\n\n";
            format << "4. **Function-by-Function Breakdown**\n";
            format << "   - **function_name()**: [Purpose, parameters, return value, algorithm]\n\n";
            format << "5. **Complexity & Performance**\n";
            format << "   - Time Complexity: [Analysis]\n";
            format << "   - Space Complexity: [Analysis]\n";
            format << "   - Performance Considerations: [Details]\n\n";
            format << "6. **Potential Issues & Improvements**\n";
            format << "   - [Issue 1]: [Explanation and suggested fix]\n";
            format << "   - [Issue 2]: [Explanation and suggested fix]\n\n";
            break;
        default:
            break;
    }

    return format.str();
}

std::string AIPromptManager::build_quality_standards_section(PromptType type) {
    std::ostringstream standards;

    switch (type) {
        case PromptType::CODE_GENERATION:
        case PromptType::CODE_EDITING:
            standards << "QUALITY ASSURANCE:\n";
            standards << "- Code should be production-ready\n";
            standards << "- Follow language-specific best practices\n";
            standards << "- Include error handling where appropriate\n";
            standards << "- Add inline comments for complex logic\n\n";
            break;
        case PromptType::CODE_DEBUGGING:
            standards << "DEBUGGING BEST PRACTICES:\n";
            standards << "- Always check return values from functions\n";
            standards << "- Validate inputs before processing\n";
            standards << "- Use proper error handling patterns\n";
            standards << "- Consider edge cases and boundary conditions\n";
            standards << "- Add logging for debugging purposes\n\n";
            break;
        case PromptType::CODE_TESTING:
            standards << "TEST QUALITY STANDARDS:\n";
            standards << "- Tests should be deterministic and repeatable\n";
            standards << "- Include clear test names and documentation\n";
            standards << "- Cover both positive and negative test cases\n";
            standards << "- Test error conditions and exception handling\n";
            standards << "- Consider performance and resource usage\n\n";
            break;
        case PromptType::CODING_ASSISTANCE:
            standards << "QUALITY STANDARDS:\n";
            standards << "- Be technically accurate and up-to-date\n";
            standards << "- Use clear, accessible language\n";
            standards << "- Provide actionable, implementable solutions\n";
            standards << "- Include working code examples\n";
            standards << "- Encourage good programming habits\n\n";
            break;
        case PromptType::CODE_EXPLANATION:
            standards << "ANALYSIS QUALITY STANDARDS:\n";
            standards << "- Be technically accurate and precise\n";
            standards << "- Explain complex concepts clearly\n";
            standards << "- Provide actionable insights and recommendations\n";
            standards << "- Consider real-world usage scenarios\n";
            standards << "- Highlight both strengths and weaknesses\n\n";
            break;
        default:
            break;
    }

    return standards.str();
}

std::string AIPromptManager::get_language_specific_guidelines(const std::string& language, PromptType type) {
    std::ostringstream guidelines;

    if (language == "cpp" || language == "c++") {
        if (type == PromptType::CODE_EDITING) {
            guidelines << "- Extract methods/functions where appropriate\n";
            guidelines << "- Improve variable and function naming\n";
            guidelines << "- Reduce code duplication\n";
            guidelines << "- Enhance readability and maintainability\n";
            guidelines << "- Apply SOLID principles where applicable\n";
        } else if (type == PromptType::CODE_EXPLANATION) {
            guidelines << "- Memory management (new/delete, smart pointers, RAII)\n";
            guidelines << "- Object lifecycle and resource cleanup\n";
            guidelines << "- Template usage and generic programming\n";
            guidelines << "- Exception safety and error handling\n";
            guidelines << "- Performance considerations (copy vs move, const correctness)\n";
        }
    } else if (language == "python") {
        if (type == PromptType::CODE_EDITING) {
            guidelines << "- Follow PEP 8 style guidelines\n";
            guidelines << "- Use list/dict comprehensions appropriately\n";
            guidelines << "- Implement proper error handling\n";
            guidelines << "- Add type hints where beneficial\n";
        } else if (type == PromptType::CODE_EXPLANATION) {
            guidelines << "- Object-oriented design and inheritance\n";
            guidelines << "- List/dict comprehensions and generator usage\n";
            guidelines << "- Exception handling and context managers\n";
            guidelines << "- Memory management (GIL, reference counting)\n";
            guidelines << "- Library usage and standard library patterns\n";
        }
    } else if (language == "javascript") {
        if (type == PromptType::CODE_EDITING) {
            guidelines << "- Use modern ES6+ features appropriately\n";
            guidelines << "- Implement proper async/await patterns\n";
            guidelines << "- Add JSDoc comments for functions\n";
            guidelines << "- Follow consistent naming conventions\n";
        } else if (type == PromptType::CODE_EXPLANATION) {
            guidelines << "- Asynchronous programming (promises, async/await)\n";
            guidelines << "- Prototype-based inheritance and closures\n";
            guidelines << "- Event-driven programming patterns\n";
            guidelines << "- Memory management and garbage collection\n";
            guidelines << "- Browser vs Node.js considerations\n";
        }
    }

    if (!guidelines.str().empty()) {
        guidelines << "\n";
    }

    return guidelines.str();
}

std::string AIPromptManager::get_framework_specific_guidelines(const std::string& framework) {
    std::ostringstream guidelines;

    if (framework == "gtest" || framework == "google test") {
        guidelines << "- Use TEST() and TEST_F() macros appropriately\n";
        guidelines << "- Leverage fixtures for setup/teardown\n";
        guidelines << "- Use ASSERT_* and EXPECT_* macros correctly\n";
        guidelines << "- Include test comments and documentation\n";
        guidelines << "- Consider parameterized tests for multiple inputs\n";
    } else if (framework == "pytest") {
        guidelines << "- Use descriptive function names with test_ prefix\n";
        guidelines << "- Leverage fixtures for setup/teardown\n";
        guidelines << "- Use assert statements with clear messages\n";
        guidelines << "- Consider parameterization for multiple inputs\n";
        guidelines << "- Include docstrings and comments\n";
    } else if (framework == "jest") {
        guidelines << "- Use describe() and it()/test() for organization\n";
        guidelines << "- Leverage beforeEach/afterEach for setup\n";
        guidelines << "- Use appropriate matchers and assertions\n";
        guidelines << "- Consider test.each() for parameterized tests\n";
        guidelines << "- Mock external dependencies properly\n";
    } else if (framework == "junit") {
        guidelines << "- Use @Test annotation and descriptive method names\n";
        guidelines << "- Leverage @BeforeEach/@AfterEach for setup\n";
        guidelines << "- Use assert methods with clear messages\n";
        guidelines << "- Consider @ParameterizedTest for multiple inputs\n";
        guidelines << "- Include proper exception testing\n";
    }

    if (!guidelines.str().empty()) {
        guidelines << "\n";
    }

    return guidelines.str();
}

std::string AIPromptManager::build_skill_level_guidance(int skill_level) {
    switch (skill_level) {
        case 1:
            return "beginner (1/5)";
        case 2:
            return "novice (2/5)";
        case 3:
            return "intermediate (3/5)";
        case 4:
            return "advanced (4/5)";
        case 5:
            return "expert (5/5)";
        default:
            return "intermediate (3/5)";
    }
}

} // namespace ai
} // namespace customos
