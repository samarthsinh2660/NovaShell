#ifndef CUSTOMOS_ENVIRONMENT_MANAGER_H
#define CUSTOMOS_ENVIRONMENT_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace env {

// Environment profile
struct EnvironmentProfile {
    std::string name;
    std::string description;
    std::map<std::string, std::string> variables;
    std::vector<std::string> paths;
    std::string sdk_path;
    std::string compiler;
    std::string python_version;
    std::string node_version;
    time_t created;
    time_t last_used;
};

// SDK information
struct SDKInfo {
    std::string name;
    std::string version;
    std::string path;
    std::string type;  // java, python, node, etc.
};

// Project Environment Manager
class EnvironmentManager {
public:
    static EnvironmentManager& instance();

    // Profile management
    bool create_profile(const std::string& name, const std::string& description = "");
    bool delete_profile(const std::string& name);
    bool switch_profile(const std::string& name);
    std::string get_current_profile() const;
    std::vector<EnvironmentProfile> list_profiles();
    EnvironmentProfile get_profile(const std::string& name);

    // Environment variables
    bool set_variable(const std::string& key, const std::string& value);
    std::string get_variable(const std::string& key);
    bool unset_variable(const std::string& key);
    std::map<std::string, std::string> get_all_variables();

    // PATH management
    bool add_to_path(const std::string& path, bool prepend = false);
    bool remove_from_path(const std::string& path);
    std::vector<std::string> get_path();

    // SDK management
    std::vector<SDKInfo> list_installed_sdks();
    bool set_java_home(const std::string& path);
    bool set_python_version(const std::string& version);
    bool set_node_version(const std::string& version);
    std::string get_active_sdk(const std::string& type);

    // Quick switch presets
    bool switch_to_java_env(const std::string& version);
    bool switch_to_python_env(const std::string& version);
    bool switch_to_node_env(const std::string& version);
    bool switch_to_cpp_env(const std::string& compiler);

    // Project-specific environments
    bool create_project_env(const std::string& project_path, const std::string& profile_name);
    bool load_project_env(const std::string& project_path);
    bool save_current_env_to_file(const std::string& filepath);
    bool load_env_from_file(const std::string& filepath);

    // Environment detection
    std::map<std::string, std::string> detect_installed_tools();

private:
    EnvironmentManager();
    ~EnvironmentManager();
    EnvironmentManager(const EnvironmentManager&) = delete;
    EnvironmentManager& operator=(const EnvironmentManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace env
} // namespace customos

#endif // CUSTOMOS_ENVIRONMENT_MANAGER_H
