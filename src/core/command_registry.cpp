#include "core/command_registry.h"
#include <algorithm>
#include <mutex>

namespace customos {
namespace core {

CommandRegistry::CommandRegistry() = default;
CommandRegistry::~CommandRegistry() = default;

bool CommandRegistry::register_command(const CommandInfo& cmd_info) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (cmd_info.name.empty() || !cmd_info.handler) {
        return false;
    }

    if (commands_.find(cmd_info.name) != commands_.end()) {
        // Command already exists
        return false;
    }

    commands_[cmd_info.name] = cmd_info;
    return true;
}

bool CommandRegistry::unregister_command(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return commands_.erase(name) > 0;
}

bool CommandRegistry::has_command(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return commands_.find(name) != commands_.end();
}

const CommandInfo* CommandRegistry::get_command(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &it->second;
    }
    return nullptr;
}

int CommandRegistry::execute(const std::string& name, const CommandContext& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = commands_.find(name);
    if (it == commands_.end()) {
        return -1; // Command not found
    }

    try {
        return it->second.handler(context);
    }
    catch (const std::exception&) {
        return -1;
    }
}

std::vector<std::string> CommandRegistry::list_commands() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> result;
    result.reserve(commands_.size());
    
    for (const auto& pair : commands_) {
        result.push_back(pair.first);
    }
    
    return result;
}

std::vector<std::string> CommandRegistry::suggest_commands(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> suggestions;
    
    for (const auto& pair : commands_) {
        if (pair.first.find(prefix) == 0) {
            suggestions.push_back(pair.first);
        }
    }
    
    std::sort(suggestions.begin(), suggestions.end());
    return suggestions;
}

} // namespace core
} // namespace customos
