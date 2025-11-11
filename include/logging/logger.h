#ifndef CUSTOMOS_LOGGER_H
#define CUSTOMOS_LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>

namespace customos {
namespace logging {

// Log levels
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Log entry
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string source;
    std::string user;
    time_t timestamp;
    std::string category;
};

// Audit event types
enum class AuditEventType {
    LOGIN,
    LOGOUT,
    COMMAND_EXECUTED,
    FILE_ACCESSED,
    PERMISSION_CHANGED,
    USER_CREATED,
    USER_DELETED,
    VAULT_ACCESSED,
    NETWORK_ACTIVITY,
    PLUGIN_LOADED,
    SYSTEM_CONFIG_CHANGED
};

// Audit entry
struct AuditEntry {
    AuditEventType event_type;
    std::string user;
    std::string action;
    std::string target;
    bool success;
    time_t timestamp;
    std::string details;
};

// Logger
class Logger {
public:
    static Logger& instance();

    // Logging functions
    void log(LogLevel level, const std::string& message, const std::string& source = "");
    void trace(const std::string& message, const std::string& source = "");
    void debug(const std::string& message, const std::string& source = "");
    void info(const std::string& message, const std::string& source = "");
    void warning(const std::string& message, const std::string& source = "");
    void error(const std::string& message, const std::string& source = "");
    void critical(const std::string& message, const std::string& source = "");

    // Audit logging
    void audit(const AuditEntry& entry);
    void audit_command(const std::string& command, bool success);
    void audit_login(const std::string& user, bool success);
    void audit_file_access(const std::string& file, const std::string& action, bool success);

    // Log retrieval
    std::vector<LogEntry> get_logs(size_t max_count = 100, LogLevel min_level = LogLevel::TRACE);
    std::vector<AuditEntry> get_audit_trail(size_t max_count = 100);
    std::vector<LogEntry> search_logs(const std::string& query, time_t start_time = 0, time_t end_time = 0);
    std::vector<AuditEntry> search_audit(const std::string& user = "", AuditEventType event_type = AuditEventType::COMMAND_EXECUTED);

    // Configuration
    void set_log_level(LogLevel level);
    LogLevel get_log_level() const;
    void set_log_file(const std::string& filepath);
    void enable_console_output(bool enable);
    void enable_file_output(bool enable);

    // Maintenance
    void flush();
    void rotate_logs();
    void clear_old_logs(int days);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Convenience macros
#define LOG_TRACE(msg) customos::logging::Logger::instance().trace(msg, __FUNCTION__)
#define LOG_DEBUG(msg) customos::logging::Logger::instance().debug(msg, __FUNCTION__)
#define LOG_INFO(msg) customos::logging::Logger::instance().info(msg, __FUNCTION__)
#define LOG_WARNING(msg) customos::logging::Logger::instance().warning(msg, __FUNCTION__)
#define LOG_ERROR(msg) customos::logging::Logger::instance().error(msg, __FUNCTION__)
#define LOG_CRITICAL(msg) customos::logging::Logger::instance().critical(msg, __FUNCTION__)

} // namespace logging
} // namespace customos

#endif // CUSTOMOS_LOGGER_H
