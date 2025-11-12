#ifndef CUSTOMOS_INTERNAL_DB_H
#define CUSTOMOS_INTERNAL_DB_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstdint>

namespace customos {
namespace database {

// Internal SQLite database for system metadata
// Used for: logs, plugin index, config, user data, history, cache
class InternalDB {
public:
    static InternalDB& instance();

    // Initialize database
    bool initialize(const std::string& db_path = "");
    bool is_initialized() const;

    // Command history
    bool add_history(const std::string& command, const std::string& user);
    std::vector<std::string> get_history(int limit = 100);
    std::vector<std::string> search_history(const std::string& query);
    bool clear_history();

    // Plugin metadata
    bool register_plugin(const std::string& name, const std::string& version, 
                         const std::string& path, bool enabled);
    bool unregister_plugin(const std::string& name);
    std::map<std::string, std::string> get_plugin_metadata(const std::string& name);
    std::vector<std::string> list_plugins();

    // Configuration storage
    bool set_config(const std::string& key, const std::string& value);
    std::string get_config(const std::string& key, const std::string& default_value = "");
    std::map<std::string, std::string> get_all_config();
    bool delete_config(const std::string& key);

    // User preferences
    bool set_user_pref(const std::string& user, const std::string& key, const std::string& value);
    std::string get_user_pref(const std::string& user, const std::string& key, const std::string& default_value = "");

    // Cache management
    bool set_cache(const std::string& key, const std::string& value, int ttl_seconds = 3600);
    std::string get_cache(const std::string& key);
    bool clear_cache();

    // Session tracking
    bool create_session(const std::string& user, const std::string& session_id);
    bool end_session(const std::string& session_id);
    std::vector<std::string> get_active_sessions();

    // Audit logging (internal)
    bool log_audit(const std::string& user, const std::string& action, const std::string& details);
    std::vector<std::string> get_audit_log(int limit = 100);

    // Database maintenance
    bool vacuum();
    bool backup(const std::string& backup_path);
    bool restore(const std::string& backup_path);
    uint64_t get_size_bytes();

private:
    InternalDB();
    ~InternalDB();
    InternalDB(const InternalDB&) = delete;
    InternalDB& operator=(const InternalDB&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace database
} // namespace customos

#endif // CUSTOMOS_INTERNAL_DB_H
