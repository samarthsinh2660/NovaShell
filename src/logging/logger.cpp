#include "logging/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <ctime>
#include <ctime>
#include <string>

namespace customos {
namespace logging {

struct Logger::Impl {
    std::vector<LogEntry> log_entries;
    std::vector<AuditEntry> audit_entries;
    LogLevel min_level = LogLevel::INFO;
    std::string log_file;
    bool console_output = true;
    bool file_output = true;
    std::mutex mutex;
    std::ofstream file_stream;

    std::string get_timestamp() {
        time_t now = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(buf);
    }

    std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
};

Logger::Logger() : pimpl_(std::make_unique<Impl>()) {
}

Logger::~Logger() {
    flush();
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& source) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (level < pimpl_->min_level) {
        return;
    }

    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.source = source;
    entry.timestamp = time(nullptr);
    entry.category = "general";

    pimpl_->log_entries.push_back(entry);

    // Format output
    std::stringstream ss;
    ss << "[" << pimpl_->get_timestamp() << "] "
       << "[" << pimpl_->level_to_string(level) << "] ";
    
    if (!source.empty()) {
        ss << "[" << source << "] ";
    }
    
    ss << message;

    // Console output
    if (pimpl_->console_output) {
        if (level >= LogLevel::ERROR) {
            std::cerr << ss.str() << std::endl;
        } else {
            std::cout << ss.str() << std::endl;
        }
    }

    // File output
    if (pimpl_->file_output && pimpl_->file_stream.is_open()) {
        pimpl_->file_stream << ss.str() << std::endl;
    }
}

void Logger::trace(const std::string& message, const std::string& source) {
    log(LogLevel::TRACE, message, source);
}

void Logger::debug(const std::string& message, const std::string& source) {
    log(LogLevel::DEBUG, message, source);
}

void Logger::info(const std::string& message, const std::string& source) {
    log(LogLevel::INFO, message, source);
}

void Logger::warning(const std::string& message, const std::string& source) {
    log(LogLevel::WARNING, message, source);
}

void Logger::error(const std::string& message, const std::string& source) {
    log(LogLevel::ERROR, message, source);
}

void Logger::critical(const std::string& message, const std::string& source) {
    log(LogLevel::CRITICAL, message, source);
}

void Logger::audit(const AuditEntry& entry) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->audit_entries.push_back(entry);

    // Also log to regular log
    std::stringstream ss;
    ss << "AUDIT: " << entry.user << " - " << entry.action 
       << " - " << entry.target 
       << " - " << (entry.success ? "SUCCESS" : "FAILED");
    
    LogEntry log_entry;
    log_entry.level = LogLevel::INFO;
    log_entry.message = ss.str();
    log_entry.timestamp = entry.timestamp;
    log_entry.category = "audit";
    
    pimpl_->log_entries.push_back(log_entry);
}

void Logger::audit_command(const std::string& command, bool success) {
    AuditEntry entry;
    entry.event_type = AuditEventType::COMMAND_EXECUTED;
    entry.action = "execute_command";
    entry.target = command;
    entry.success = success;
    entry.timestamp = time(nullptr);
    
    audit(entry);
}

void Logger::audit_login(const std::string& user, bool success) {
    AuditEntry entry;
    entry.event_type = success ? AuditEventType::LOGIN : AuditEventType::LOGOUT;
    entry.user = user;
    entry.action = success ? "login" : "login_failed";
    entry.success = success;
    entry.timestamp = time(nullptr);
    
    audit(entry);
}

void Logger::audit_file_access(const std::string& file, const std::string& action, bool success) {
    AuditEntry entry;
    entry.event_type = AuditEventType::FILE_ACCESSED;
    entry.action = action;
    entry.target = file;
    entry.success = success;
    entry.timestamp = time(nullptr);
    
    audit(entry);
}

std::vector<LogEntry> Logger::get_logs(size_t max_count, LogLevel min_level) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<LogEntry> result;
    for (auto it = pimpl_->log_entries.rbegin(); it != pimpl_->log_entries.rend() && result.size() < max_count; ++it) {
        if (it->level >= min_level) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<AuditEntry> Logger::get_audit_trail(size_t max_count) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<AuditEntry> result;
    size_t start = pimpl_->audit_entries.size() > max_count ? 
                   pimpl_->audit_entries.size() - max_count : 0;
    
    for (size_t i = start; i < pimpl_->audit_entries.size(); ++i) {
        result.push_back(pimpl_->audit_entries[i]);
    }
    
    return result;
}

std::vector<LogEntry> Logger::search_logs(const std::string& query, time_t start_time, time_t end_time) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<LogEntry> result;
    for (const auto& entry : pimpl_->log_entries) {
        if ((start_time == 0 || entry.timestamp >= start_time) &&
            (end_time == 0 || entry.timestamp <= end_time) &&
            entry.message.find(query) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<AuditEntry> Logger::search_audit(const std::string& user, AuditEventType event_type) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<AuditEntry> result;
    for (const auto& entry : pimpl_->audit_entries) {
        if ((user.empty() || entry.user == user)) {
            result.push_back(entry);
        }
    }
    
    return result;
}

void Logger::set_log_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->min_level = level;
}

LogLevel Logger::get_log_level() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->min_level;
}

void Logger::set_log_file(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->file_stream.is_open()) {
        pimpl_->file_stream.close();
    }
    
    pimpl_->log_file = filepath;
    pimpl_->file_stream.open(filepath, std::ios::app);
}

void Logger::enable_console_output(bool enable) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->console_output = enable;
}

void Logger::enable_file_output(bool enable) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->file_output = enable;
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    if (pimpl_->file_stream.is_open()) {
        pimpl_->file_stream.flush();
    }
}

void Logger::rotate_logs() {
    // TODO: Implement log rotation
}

void Logger::clear_old_logs(int days) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    time_t cutoff = time(nullptr) - (days * 24 * 60 * 60);
    
    pimpl_->log_entries.erase(
        std::remove_if(pimpl_->log_entries.begin(), pimpl_->log_entries.end(),
            [cutoff](const LogEntry& entry) { return entry.timestamp < cutoff; }),
        pimpl_->log_entries.end()
    );
    
    pimpl_->audit_entries.erase(
        std::remove_if(pimpl_->audit_entries.begin(), pimpl_->audit_entries.end(),
            [cutoff](const AuditEntry& entry) { return entry.timestamp < cutoff; }),
        pimpl_->audit_entries.end()
    );
}

} // namespace logging
} // namespace customos
