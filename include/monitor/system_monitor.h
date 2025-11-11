#ifndef CUSTOMOS_SYSTEM_MONITOR_H
#define CUSTOMOS_SYSTEM_MONITOR_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace monitor {

// CPU information
struct CPUInfo {
    std::string model;
    int cores;
    int threads;
    float usage_percent;  // Overall CPU usage
    std::vector<float> core_usage;  // Per-core usage
    float temperature;  // Celsius
};

// Memory information
struct MemoryInfo {
    uint64_t total_bytes;
    uint64_t available_bytes;
    uint64_t used_bytes;
    float usage_percent;
    uint64_t swap_total;
    uint64_t swap_used;
};

// Disk information
struct DiskInfo {
    std::string mount_point;
    std::string device;
    std::string filesystem;
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint64_t available_bytes;
    float usage_percent;
    uint64_t read_bytes;
    uint64_t write_bytes;
};

// Network interface information
struct NetworkInfo {
    std::string interface;
    std::string ip_address;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t packets_sent;
    uint64_t packets_received;
    float upload_speed;  // bytes/sec
    float download_speed;  // bytes/sec
};

// Process information
struct ProcessInfo {
    int pid;
    std::string name;
    std::string user;
    float cpu_percent;
    uint64_t memory_bytes;
    std::string status;
    std::string command;
};

// Real-Time System Monitor
class SystemMonitor {
public:
    static SystemMonitor& instance();

    // Start/Stop monitoring
    bool start_monitoring(int update_interval_ms = 1000);
    void stop_monitoring();
    bool is_monitoring() const;

    // System information
    CPUInfo get_cpu_info();
    MemoryInfo get_memory_info();
    std::vector<DiskInfo> get_disk_info();
    std::vector<NetworkInfo> get_network_info();
    
    // Process management
    std::vector<ProcessInfo> get_processes(bool sort_by_cpu = true);
    ProcessInfo get_process_info(int pid);
    bool kill_process(int pid, bool force = false);

    // System stats
    std::map<std::string, std::string> get_system_info();
    float get_uptime_hours();
    int get_process_count();
    int get_thread_count();

    // Alerts and thresholds
    void set_cpu_threshold(float percent);
    void set_memory_threshold(float percent);
    void set_disk_threshold(float percent);
    
    using AlertCallback = std::function<void(const std::string& alert_message)>;
    void set_alert_callback(AlertCallback callback);

    // Historical data (last N samples)
    std::vector<float> get_cpu_history(int samples = 60);
    std::vector<float> get_memory_history(int samples = 60);

private:
    SystemMonitor();
    ~SystemMonitor();
    SystemMonitor(const SystemMonitor&) = delete;
    SystemMonitor& operator=(const SystemMonitor&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace monitor
} // namespace customos

#endif // CUSTOMOS_SYSTEM_MONITOR_H
