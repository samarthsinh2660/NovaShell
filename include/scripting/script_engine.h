#ifndef CUSTOMOS_SCRIPT_ENGINE_H
#define CUSTOMOS_SCRIPT_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace customos {
namespace scripting {

// Script variable types
enum class VarType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    ARRAY,
    OBJECT
};

// Script variable
struct Variable {
    VarType type;
    std::string value;
};

// Script execution result
struct ScriptResult {
    bool success;
    std::string output;
    std::string error;
    int exit_code;
};

// Script context
struct ScriptContext {
    std::map<std::string, Variable> variables;
    std::string working_directory;
    std::string current_user;
};

// Script Engine
class ScriptEngine {
public:
    static ScriptEngine& instance();

    // Script execution
    ScriptResult execute(const std::string& script_content);
    ScriptResult execute_file(const std::string& filepath);
    ScriptResult execute_line(const std::string& line);

    // Variable management
    void set_variable(const std::string& name, const Variable& value);
    Variable get_variable(const std::string& name) const;
    bool has_variable(const std::string& name) const;
    void clear_variables();

    // Function registration
    using ScriptFunction = std::function<ScriptResult(const std::vector<Variable>&)>;
    void register_function(const std::string& name, ScriptFunction func);
    void unregister_function(const std::string& name);

    // Macro management
    bool create_macro(const std::string& name, const std::string& content);
    bool delete_macro(const std::string& name);
    std::vector<std::string> list_macros() const;
    ScriptResult execute_macro(const std::string& name);

    // Script validation
    bool validate_syntax(const std::string& script_content, std::string& error);

    // Context management
    void set_context(const ScriptContext& context);
    ScriptContext get_context() const;

private:
    ScriptEngine();
    ~ScriptEngine();
    ScriptEngine(const ScriptEngine&) = delete;
    ScriptEngine& operator=(const ScriptEngine&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace scripting
} // namespace customos

#endif // CUSTOMOS_SCRIPT_ENGINE_H
