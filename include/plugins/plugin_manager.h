#ifndef CUSTOMOS_PLUGIN_MANAGER_H
#define CUSTOMOS_PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace customos {
namespace plugins {

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual std::string get_name() const = 0;
    virtual std::string get_version() const = 0;
    virtual std::string get_description() const = 0;
    virtual std::string get_author() const = 0;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual std::vector<std::string> get_commands() const = 0;
};

// Plugin information
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string filepath;
    std::vector<std::string> commands;
    bool loaded;
};

// Plugin Manager
class PluginManager {
public:
    static PluginManager& instance();

    // Plugin loading
    bool load_plugin(const std::string& filepath);
    bool unload_plugin(const std::string& name);
    bool reload_plugin(const std::string& name);

    // Plugin queries
    bool is_loaded(const std::string& name) const;
    std::vector<PluginInfo> list_plugins() const;
    PluginInfo get_plugin_info(const std::string& name) const;

    // Plugin directory management
    void add_plugin_directory(const std::string& directory);
    std::vector<std::string> get_plugin_directories() const;
    std::vector<std::string> scan_plugins() const;

    // Auto-load
    bool enable_autoload(const std::string& name);
    bool disable_autoload(const std::string& name);
    void load_autoload_plugins();

private:
    PluginManager();
    ~PluginManager();
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Plugin export macro
#define EXPORT_PLUGIN(PluginClass) \
    extern "C" { \
        IPlugin* create_plugin() { \
            return new PluginClass(); \
        } \
        void destroy_plugin(IPlugin* plugin) { \
            delete plugin; \
        } \
    }

} // namespace plugins
} // namespace customos

#endif // CUSTOMOS_PLUGIN_MANAGER_H
