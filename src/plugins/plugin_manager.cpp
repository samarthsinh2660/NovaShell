#include "plugins/plugin_manager.h"
#include <map>
#include <mutex>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace customos {
namespace plugins {

struct LoadedPlugin {
    void* handle;
    IPlugin* instance;
    PluginInfo info;
};

struct PluginManager::Impl {
    std::map<std::string, LoadedPlugin> plugins;
    std::vector<std::string> plugin_directories;
    std::vector<std::string> autoload_plugins;
    std::mutex mutex;
};

PluginManager::PluginManager() : pimpl_(std::make_unique<Impl>()) {
}

PluginManager::~PluginManager() {
    // Unload all plugins
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    for (auto& pair : pimpl_->plugins) {
        if (pair.second.instance) {
            pair.second.instance->shutdown();
            // Call destroy function
        }
#ifdef _WIN32
        if (pair.second.handle) {
            FreeLibrary((HMODULE)pair.second.handle);
        }
#else
        if (pair.second.handle) {
            dlclose(pair.second.handle);
        }
#endif
    }
}

PluginManager& PluginManager::instance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::load_plugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    // TODO: Implement actual dynamic library loading
    // For now, just track the request
    return false;
}

bool PluginManager::unload_plugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->plugins.find(name);
    if (it == pimpl_->plugins.end()) {
        return false;
    }

    // Shutdown plugin
    if (it->second.instance) {
        it->second.instance->shutdown();
    }

    // Unload library
#ifdef _WIN32
    if (it->second.handle) {
        FreeLibrary((HMODULE)it->second.handle);
    }
#else
    if (it->second.handle) {
        dlclose(it->second.handle);
    }
#endif

    pimpl_->plugins.erase(it);
    return true;
}

bool PluginManager::reload_plugin(const std::string& name) {
    auto it = pimpl_->plugins.find(name);
    if (it == pimpl_->plugins.end()) {
        return false;
    }

    std::string filepath = it->second.info.filepath;
    unload_plugin(name);
    return load_plugin(filepath);
}

bool PluginManager::is_loaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->plugins.find(name) != pimpl_->plugins.end();
}

std::vector<PluginInfo> PluginManager::list_plugins() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<PluginInfo> result;
    for (const auto& pair : pimpl_->plugins) {
        result.push_back(pair.second.info);
    }
    return result;
}

PluginInfo PluginManager::get_plugin_info(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->plugins.find(name);
    if (it != pimpl_->plugins.end()) {
        return it->second.info;
    }
    return PluginInfo{};
}

void PluginManager::add_plugin_directory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->plugin_directories.push_back(directory);
}

std::vector<std::string> PluginManager::get_plugin_directories() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->plugin_directories;
}

std::vector<std::string> PluginManager::scan_plugins() const {
    // TODO: Scan directories for plugin files
    return {};
}

bool PluginManager::enable_autoload(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->autoload_plugins.push_back(name);
    return true;
}

bool PluginManager::disable_autoload(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    auto it = std::find(pimpl_->autoload_plugins.begin(), pimpl_->autoload_plugins.end(), name);
    if (it != pimpl_->autoload_plugins.end()) {
        pimpl_->autoload_plugins.erase(it);
        return true;
    }
    return false;
}

void PluginManager::load_autoload_plugins() {
    std::vector<std::string> to_load;
    {
        std::lock_guard<std::mutex> lock(pimpl_->mutex);
        to_load = pimpl_->autoload_plugins;
    }

    for (const auto& name : to_load) {
        load_plugin(name);
    }
}

} // namespace plugins
} // namespace customos
