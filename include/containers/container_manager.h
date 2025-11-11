#ifndef CUSTOMOS_CONTAINER_MANAGER_H
#define CUSTOMOS_CONTAINER_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace containers {

// Container status
enum class ContainerStatus {
    RUNNING,
    STOPPED,
    PAUSED,
    RESTARTING,
    DEAD,
    UNKNOWN
};

// Container information
struct ContainerInfo {
    std::string id;
    std::string name;
    std::string image;
    ContainerStatus status;
    std::vector<std::string> ports;
    std::map<std::string, std::string> labels;
    time_t created;
    std::string command;
};

// Image information
struct ImageInfo {
    std::string id;
    std::string repository;
    std::string tag;
    uint64_t size;
    time_t created;
};

// Container Manager (Docker/Podman integration)
class ContainerManager {
public:
    static ContainerManager& instance();

    // Initialize connection to container runtime
    bool initialize(const std::string& socket_path = "");
    bool is_initialized() const;

    // Container operations
    std::string create_container(const std::string& image, const std::string& name, 
                                  const std::map<std::string, std::string>& options = {});
    bool start_container(const std::string& container_id);
    bool stop_container(const std::string& container_id);
    bool restart_container(const std::string& container_id);
    bool remove_container(const std::string& container_id, bool force = false);
    bool pause_container(const std::string& container_id);
    bool unpause_container(const std::string& container_id);

    // Container queries
    std::vector<ContainerInfo> list_containers(bool all = false);
    ContainerInfo get_container_info(const std::string& container_id);
    std::string get_container_logs(const std::string& container_id, int tail = 100);
    
    // Execute command in container
    std::string exec_in_container(const std::string& container_id, const std::vector<std::string>& command);

    // Image operations
    std::vector<ImageInfo> list_images();
    bool pull_image(const std::string& image_name);
    bool remove_image(const std::string& image_id);
    bool build_image(const std::string& dockerfile_path, const std::string& tag);

    // Network operations
    std::vector<std::string> list_networks();
    std::string create_network(const std::string& name);
    bool remove_network(const std::string& network_id);

    // Volume operations
    std::vector<std::string> list_volumes();
    std::string create_volume(const std::string& name);
    bool remove_volume(const std::string& volume_name);

    // System information
    std::map<std::string, std::string> get_system_info();

private:
    ContainerManager();
    ~ContainerManager();
    ContainerManager(const ContainerManager&) = delete;
    ContainerManager& operator=(const ContainerManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace containers
} // namespace customos

#endif // CUSTOMOS_CONTAINER_MANAGER_H
