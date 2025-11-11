#include "database/internal_db.h"
#include <sqlite3.h>
#include <mutex>
#include <sstream>

namespace customos {
namespace database {

struct InternalDB::Impl {
    sqlite3* db = nullptr;
    std::string db_path;
    bool initialized = false;
    std::mutex mutex;

    bool execute(const std::string& sql) {
        char* err_msg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            if (err_msg) {
                sqlite3_free(err_msg);
            }
            return false;
        }
        return true;
    }

    bool create_tables() {
        // Command history table
        execute(R"(
            CREATE TABLE IF NOT EXISTS history (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                command TEXT NOT NULL,
                user TEXT,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Plugin metadata table
        execute(R"(
            CREATE TABLE IF NOT EXISTS plugins (
                name TEXT PRIMARY KEY,
                version TEXT,
                path TEXT,
                enabled INTEGER DEFAULT 1,
                registered_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Configuration table
        execute(R"(
            CREATE TABLE IF NOT EXISTS config (
                key TEXT PRIMARY KEY,
                value TEXT,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // User preferences table
        execute(R"(
            CREATE TABLE IF NOT EXISTS user_prefs (
                user TEXT,
                key TEXT,
                value TEXT,
                PRIMARY KEY (user, key)
            )
        )");

        // Cache table
        execute(R"(
            CREATE TABLE IF NOT EXISTS cache (
                key TEXT PRIMARY KEY,
                value TEXT,
                expires_at DATETIME
            )
        )");

        // Sessions table
        execute(R"(
            CREATE TABLE IF NOT EXISTS sessions (
                session_id TEXT PRIMARY KEY,
                user TEXT,
                started_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                ended_at DATETIME
            )
        )");

        // Audit log table
        execute(R"(
            CREATE TABLE IF NOT EXISTS audit_log (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user TEXT,
                action TEXT,
                details TEXT,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        return true;
    }
};

InternalDB::InternalDB() : pimpl_(std::make_unique<Impl>()) {
}

InternalDB::~InternalDB() {
    if (pimpl_->db) {
        sqlite3_close(pimpl_->db);
    }
}

InternalDB& InternalDB::instance() {
    static InternalDB instance;
    return instance;
}

bool InternalDB::initialize(const std::string& db_path) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->initialized) {
        return true;
    }

    pimpl_->db_path = db_path.empty() ? ".customos/system.db" : db_path;
    
    int rc = sqlite3_open(pimpl_->db_path.c_str(), &pimpl_->db);
    if (rc != SQLITE_OK) {
        return false;
    }

    pimpl_->create_tables();
    pimpl_->initialized = true;
    
    return true;
}

bool InternalDB::is_initialized() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->initialized;
}

bool InternalDB::add_history(const std::string& command, const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO history (command, user) VALUES (?, ?)";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, command.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::vector<std::string> InternalDB::get_history(int limit) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<std::string> history;
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT command FROM history ORDER BY timestamp DESC LIMIT " + std::to_string(limit);
    
    if (sqlite3_prepare_v2(pimpl_->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return history;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* cmd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (cmd) {
            history.push_back(cmd);
        }
    }

    sqlite3_finalize(stmt);
    return history;
}

std::vector<std::string> InternalDB::search_history(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<std::string> results;
    sqlite3_stmt* stmt;
    
    const char* sql = "SELECT command FROM history WHERE command LIKE ? ORDER BY timestamp DESC";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return results;
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* cmd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (cmd) {
            results.push_back(cmd);
        }
    }

    sqlite3_finalize(stmt);
    return results;
}

bool InternalDB::clear_history() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->execute("DELETE FROM history");
}

bool InternalDB::register_plugin(const std::string& name, const std::string& version, 
                                  const std::string& path, bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO plugins (name, version, path, enabled) VALUES (?, ?, ?, ?)";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, enabled ? 1 : 0);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool InternalDB::unregister_plugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM plugins WHERE name = ?";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::map<std::string, std::string> InternalDB::get_plugin_metadata(const std::string& name) {
    // TODO: Implement
    return {};
}

std::vector<std::string> InternalDB::list_plugins() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<std::string> plugins;
    sqlite3_stmt* stmt;
    
    const char* sql = "SELECT name FROM plugins WHERE enabled = 1";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return plugins;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (name) {
            plugins.push_back(name);
        }
    }

    sqlite3_finalize(stmt);
    return plugins;
}

bool InternalDB::set_config(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO config (key, value) VALUES (?, ?)";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::string InternalDB::get_config(const std::string& key, const std::string& default_value) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT value FROM config WHERE key = ?";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return default_value;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    
    std::string value = default_value;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (val) {
            value = val;
        }
    }

    sqlite3_finalize(stmt);
    return value;
}

std::map<std::string, std::string> InternalDB::get_all_config() {
    // TODO: Implement
    return {};
}

bool InternalDB::delete_config(const std::string& key) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM config WHERE key = ?";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool InternalDB::set_user_pref(const std::string& user, const std::string& key, const std::string& value) {
    // TODO: Implement
    return false;
}

std::string InternalDB::get_user_pref(const std::string& user, const std::string& key, const std::string& default_value) {
    // TODO: Implement
    return default_value;
}

bool InternalDB::set_cache(const std::string& key, const std::string& value, int ttl_seconds) {
    // TODO: Implement with expiration
    return false;
}

std::string InternalDB::get_cache(const std::string& key) {
    // TODO: Implement with expiration check
    return "";
}

bool InternalDB::clear_cache() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->execute("DELETE FROM cache");
}

bool InternalDB::create_session(const std::string& user, const std::string& session_id) {
    // TODO: Implement
    return false;
}

bool InternalDB::end_session(const std::string& session_id) {
    // TODO: Implement
    return false;
}

std::vector<std::string> InternalDB::get_active_sessions() {
    // TODO: Implement
    return {};
}

bool InternalDB::log_audit(const std::string& user, const std::string& action, const std::string& details) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO audit_log (user, action, details) VALUES (?, ?, ?)";
    
    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, details.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::vector<std::string> InternalDB::get_audit_log(int limit) {
    // TODO: Implement
    return {};
}

bool InternalDB::vacuum() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->execute("VACUUM");
}

bool InternalDB::backup(const std::string& backup_path) {
    // TODO: Implement SQLite backup API
    return false;
}

bool InternalDB::restore(const std::string& backup_path) {
    // TODO: Implement
    return false;
}

uint64_t InternalDB::get_size_bytes() {
    // TODO: Implement
    return 0;
}

} // namespace database
} // namespace customos
