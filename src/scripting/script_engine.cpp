#include "scripting/script_engine.h"
#include <map>
#include <mutex>
#include <sstream>

namespace customos {
namespace scripting {

struct ScriptEngine::Impl {
    std::map<std::string, Variable> variables;
    std::map<std::string, ScriptFunction> functions;
    std::map<std::string, std::string> macros;
    ScriptContext context;
    std::mutex mutex;
};

ScriptEngine::ScriptEngine() : pimpl_(std::make_unique<Impl>()) {
}

ScriptEngine::~ScriptEngine() = default;

ScriptEngine& ScriptEngine::instance() {
    static ScriptEngine instance;
    return instance;
}

ScriptResult ScriptEngine::execute(const std::string& script_content) {
    ScriptResult result;
    result.success = false;
    result.exit_code = 1;

    // TODO: Implement script execution
    // For now, just execute line by line
    std::istringstream iss(script_content);
    std::string line;
    
    while (std::getline(iss, line)) {
        auto line_result = execute_line(line);
        result.output += line_result.output;
        if (!line_result.success) {
            result.error = line_result.error;
            return result;
        }
    }

    result.success = true;
    result.exit_code = 0;
    return result;
}

ScriptResult ScriptEngine::execute_file(const std::string& filepath) {
    // TODO: Read file and execute
    ScriptResult result;
    result.error = "File execution not implemented";
    result.success = false;
    result.exit_code = 1;
    return result;
}

ScriptResult ScriptEngine::execute_line(const std::string& line) {
    ScriptResult result;
    result.success = true;
    result.exit_code = 0;

    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
        return result;
    }

    // TODO: Parse and execute script commands
    result.output = "Script execution: " + line + "\n";
    return result;
}

void ScriptEngine::set_variable(const std::string& name, const Variable& value) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->variables[name] = value;
}

Variable ScriptEngine::get_variable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    auto it = pimpl_->variables.find(name);
    if (it != pimpl_->variables.end()) {
        return it->second;
    }
    return Variable{};
}

bool ScriptEngine::has_variable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->variables.find(name) != pimpl_->variables.end();
}

void ScriptEngine::clear_variables() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->variables.clear();
}

void ScriptEngine::register_function(const std::string& name, ScriptFunction func) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->functions[name] = func;
}

void ScriptEngine::unregister_function(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->functions.erase(name);
}

bool ScriptEngine::create_macro(const std::string& name, const std::string& content) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->macros[name] = content;
    return true;
}

bool ScriptEngine::delete_macro(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->macros.erase(name) > 0;
}

std::vector<std::string> ScriptEngine::list_macros() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    std::vector<std::string> result;
    for (const auto& pair : pimpl_->macros) {
        result.push_back(pair.first);
    }
    return result;
}

ScriptResult ScriptEngine::execute_macro(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->macros.find(name);
    if (it == pimpl_->macros.end()) {
        ScriptResult result;
        result.error = "Macro not found: " + name;
        result.success = false;
        result.exit_code = 1;
        return result;
    }

    // Unlock before executing to avoid deadlock
    std::string content = it->second;
    pimpl_->mutex.unlock();
    
    auto result = execute(content);
    
    pimpl_->mutex.lock();
    return result;
}

bool ScriptEngine::validate_syntax(const std::string& script_content, std::string& error) {
    // TODO: Implement syntax validation
    error.clear();
    return true;
}

void ScriptEngine::set_context(const ScriptContext& context) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->context = context;
}

ScriptContext ScriptEngine::get_context() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->context;
}

} // namespace scripting
} // namespace customos
