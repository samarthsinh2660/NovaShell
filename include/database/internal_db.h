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

    // User management
    bool create_user(const std::string& username, const std::string& password_hash,
                    const std::string& salt, const std::string& role,
                    uint32_t permissions, const std::string& home_directory);
    bool update_user(const std::string& username, const std::string& password_hash,
                    const std::string& salt, uint32_t permissions);
    bool delete_user(const std::string& username);
    std::map<std::string, std::string> get_user(const std::string& username);
    std::vector<std::map<std::string, std::string>> list_users();
    bool user_exists(const std::string& username);

    // Vault operations
    bool initialize_vault(const std::string& user, const std::string& master_key_hash, const std::string& salt);
    bool is_vault_initialized(const std::string& user);
    std::map<std::string, std::string> get_vault_key(const std::string& user);
    bool add_vault_password(const std::string& user, const std::string& service,
                           const std::string& username, const std::string& password,
                           const std::string& url, const std::string& notes);
    bool update_vault_password(const std::string& user, const std::string& service,
                              const std::string& username, const std::string& password,
                              const std::string& url, const std::string& notes);
    bool delete_vault_password(const std::string& user, const std::string& service);
    std::map<std::string, std::string> get_vault_password(const std::string& user, const std::string& service);
    std::vector<std::map<std::string, std::string>> list_vault_passwords(const std::string& user);
    std::vector<std::map<std::string, std::string>> search_vault_passwords(const std::string& user, const std::string& query);
    bool clear_vault(const std::string& user);

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

    // Notes and Snippets
    bool add_note(const std::string& user, const std::string& id, const std::string& title,
                 const std::string& content, const std::string& category,
                 const std::vector<std::string>& tags);
    bool update_note(const std::string& user, const std::string& id, const std::string& title,
                    const std::string& content, const std::string& category,
                    const std::vector<std::string>& tags);
    bool delete_note(const std::string& user, const std::string& id);
    std::vector<std::map<std::string, std::string>> get_notes(const std::string& user, const std::string& category = "");
    std::vector<std::map<std::string, std::string>> search_notes(const std::string& user, const std::string& query);

    // Code Snippets
    bool add_snippet(const std::string& user, const std::string& id, const std::string& title,
                    const std::string& code, const std::string& language,
                    const std::string& description, const std::vector<std::string>& tags);
    bool update_snippet(const std::string& user, const std::string& id, const std::string& title,
                       const std::string& code, const std::string& language,
                       const std::string& description, const std::vector<std::string>& tags);
    bool delete_snippet(const std::string& user, const std::string& id);
    std::vector<std::map<std::string, std::string>> get_snippets(const std::string& user, const std::string& language = "");
    std::vector<std::map<std::string, std::string>> search_snippets(const std::string& user, const std::string& query);

    // Task Scheduler
    bool add_scheduled_task(const std::string& user, const std::string& id, const std::string& title,
                           const std::string& command, const std::string& schedule);
    bool update_scheduled_task(const std::string& user, const std::string& id, bool enabled);
    bool delete_scheduled_task(const std::string& user, const std::string& id);
    std::vector<std::map<std::string, std::string>> get_scheduled_tasks(const std::string& user);

    // Reminders
    bool add_reminder(const std::string& user, const std::string& id, const std::string& title,
                     const std::string& message, const std::string& remind_at);
    bool complete_reminder(const std::string& user, const std::string& id);
    bool delete_reminder(const std::string& user, const std::string& id);
    std::vector<std::map<std::string, std::string>> get_pending_reminders(const std::string& user);
    std::vector<std::map<std::string, std::string>> get_all_reminders(const std::string& user);

    // Environment profiles
    bool add_environment_profile(const std::string& user, const std::string& name,
                                const std::string& description, const std::map<std::string, std::string>& config);
    bool update_environment_profile(const std::string& user, const std::string& name);
    bool delete_environment_profile(const std::string& user, const std::string& name);
    std::vector<std::map<std::string, std::string>> get_environment_profiles(const std::string& user);

    // Analytics data
    bool add_analytics_data(const std::string& metric_name, double value,
                           const std::string& unit, const std::string& metadata = "");
    std::vector<std::map<std::string, std::string>> get_analytics_data(const std::string& metric_name, int limit = 100);
    std::map<std::string, int> get_analytics_summary();

    // P2P file sharing
    bool add_p2p_share(const std::string& user, const std::string& share_id,
                      const std::string& filename, const std::string& filepath,
                      long long file_size, const std::string& checksum, bool is_public);
    bool record_p2p_download(const std::string& share_id, const std::string& downloader_ip);
    std::vector<std::map<std::string, std::string>> get_p2p_shares(const std::string& user);
    bool delete_p2p_share(const std::string& user, const std::string& share_id);

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
