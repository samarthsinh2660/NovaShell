#include "monitor/system_monitor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#endif

namespace customos {
namespace monitor {

struct SystemMonitor::Impl {
    bool monitoring = false;
    int update_interval_ms = 1000;
    std::thread monitor_thread;
    std::mutex mutex;
    
    CPUInfo cpu_info{};
    MemoryInfo memory_info{};
    std::vector<DiskInfo> disk_info;
    std::vector<NetworkInfo> network_info;
    
    float cpu_threshold = 90.0f;
    float memory_threshold = 90.0f;
    float disk_threshold = 90.0f;
    
    std::function<void(const std::string&)> alert_callback;
    
    std::vector<float> cpu_history;
    std::vector<float> memory_history;

    void monitor_loop() {
        while (monitoring) {
            update_stats();
            std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_ms));
        }
    }

    void update_stats() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Update CPU info
        cpu_info.usage_percent = get_cpu_usage();
        cpu_history.push_back(cpu_info.usage_percent);
        if (cpu_history.size() > 60) cpu_history.erase(cpu_history.begin());
        
        // Update Memory info
        update_memory_info();
        memory_history.push_back(memory_info.usage_percent);
        if (memory_history.size() > 60) memory_history.erase(memory_history.begin());
        
        // Check thresholds
        check_thresholds();
    }

    float get_cpu_usage() {
#ifdef _WIN32
        // TODO: Implement Windows CPU usage
        return 0.0f;
#else
        // TODO: Implement Linux/Mac CPU usage
        return 0.0f;
#endif
    }

    void update_memory_info() {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        
        memory_info.total_bytes = memInfo.ullTotalPhys;
        memory_info.available_bytes = memInfo.ullAvailPhys;
        memory_info.used_bytes = memory_info.total_bytes - memory_info.available_bytes;
        memory_info.usage_percent = (float)memory_info.used_bytes / memory_info.total_bytes * 100.0f;
#else
        struct sysinfo si;
        sysinfo(&si);
        
        memory_info.total_bytes = si.totalram * si.mem_unit;
        memory_info.available_bytes = si.freeram * si.mem_unit;
        memory_info.used_bytes = memory_info.total_bytes - memory_info.available_bytes;
        memory_info.usage_percent = (float)memory_info.used_bytes / memory_info.total_bytes * 100.0f;
#endif
    }

    void check_thresholds() {
        if (alert_callback) {
            if (cpu_info.usage_percent > cpu_threshold) {
                alert_callback("CPU usage exceeded threshold: " + std::to_string(cpu_info.usage_percent) + "%");
            }
            if (memory_info.usage_percent > memory_threshold) {
                alert_callback("Memory usage exceeded threshold: " + std::to_string(memory_info.usage_percent) + "%");
            }
        }
    }
};

SystemMonitor::SystemMonitor() : pimpl_(std::make_unique<Impl>()) {
}

SystemMonitor::~SystemMonitor() {
    stop_monitoring();
}

SystemMonitor& SystemMonitor::instance() {
    static SystemMonitor instance;
    return instance;
}

bool SystemMonitor::start_monitoring(int update_interval_ms) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->monitoring) {
        return false;
    }

    pimpl_->update_interval_ms = update_interval_ms;
    pimpl_->monitoring = true;
    pimpl_->monitor_thread = std::thread(&Impl::monitor_loop, pimpl_.get());
    
    return true;
}

void SystemMonitor::stop_monitoring() {
    {
        std::lock_guard<std::mutex> lock(pimpl_->mutex);
        pimpl_->monitoring = false;
    }
    
    if (pimpl_->monitor_thread.joinable()) {
        pimpl_->monitor_thread.join();
    }
}

bool SystemMonitor::is_monitoring() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->monitoring;
}

CPUInfo SystemMonitor::get_cpu_info() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->cpu_info;
}

MemoryInfo SystemMonitor::get_memory_info() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->memory_info;
}

std::vector<DiskInfo> SystemMonitor::get_disk_info() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->disk_info;
}

std::vector<NetworkInfo> SystemMonitor::get_network_info() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->network_info;
}

std::vector<ProcessInfo> SystemMonitor::get_processes(bool sort_by_cpu) {
    // TODO: Implement process enumeration
    return {};
}

ProcessInfo SystemMonitor::get_process_info(int pid) {
    return ProcessInfo{};
}

bool SystemMonitor::kill_process(int pid, bool force) {
    // TODO: Implement process termination
    return false;
}

std::map<std::string, std::string> SystemMonitor::get_system_info() {
    std::map<std::string, std::string> info;
    // TODO: Gather system info
    return info;
}

float SystemMonitor::get_uptime_hours() {
#ifdef _WIN32
    return GetTickCount64() / (1000.0f * 3600.0f);
#else
    struct sysinfo si;
    sysinfo(&si);
    return si.uptime / 3600.0f;
#endif
}

int SystemMonitor::get_process_count() {
    // TODO: Count processes
    return 0;
}

int SystemMonitor::get_thread_count() {
    // TODO: Count threads
    return 0;
}

void SystemMonitor::set_cpu_threshold(float percent) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->cpu_threshold = percent;
}

void SystemMonitor::set_memory_threshold(float percent) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->memory_threshold = percent;
}

void SystemMonitor::set_disk_threshold(float percent) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->disk_threshold = percent;
}

void SystemMonitor::set_alert_callback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->alert_callback = callback;
}

std::vector<float> SystemMonitor::get_cpu_history(int samples) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->cpu_history.size() <= samples) {
        return pimpl_->cpu_history;
    }
    
    return std::vector<float>(
        pimpl_->cpu_history.end() - samples,
        pimpl_->cpu_history.end()
    );
}

std::vector<float> SystemMonitor::get_memory_history(int samples) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->memory_history.size() <= samples) {
        return pimpl_->memory_history;
    }
    
    return std::vector<float>(
        pimpl_->memory_history.end() - samples,
        pimpl_->memory_history.end()
    );
}

} // namespace monitor
} // namespace customos
