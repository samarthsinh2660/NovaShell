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

        // Users table for authentication
        execute(R"(
            CREATE TABLE IF NOT EXISTS users (
                username TEXT PRIMARY KEY,
                password_hash TEXT NOT NULL,
                salt TEXT NOT NULL,
                role TEXT NOT NULL DEFAULT 'user',
                permissions INTEGER DEFAULT 0,
                active INTEGER DEFAULT 1,
                home_directory TEXT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Vault passwords table (encrypted)
        execute(R"(
            CREATE TABLE IF NOT EXISTS vault_passwords (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user TEXT NOT NULL,
                service TEXT NOT NULL,
                username TEXT,
                password TEXT NOT NULL,
                url TEXT,
                notes TEXT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                UNIQUE(user, service)
            )
        )");

        // Vault master keys (encrypted)
        execute(R"(
            CREATE TABLE IF NOT EXISTS vault_keys (
                user TEXT PRIMARY KEY,
                master_key_hash TEXT NOT NULL,
                salt TEXT NOT NULL,
                initialized_at DATETIME DEFAULT CURRENT_TIMESTAMP
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

        // Notes table
        execute(R"(
            CREATE TABLE IF NOT EXISTS notes (
                id TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                title TEXT NOT NULL,
                content TEXT,
                category TEXT,
                tags TEXT,  -- JSON array of tags
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                modified_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Code snippets table
        execute(R"(
            CREATE TABLE IF NOT EXISTS code_snippets (
                id TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                title TEXT NOT NULL,
                code TEXT NOT NULL,
                language TEXT,
                description TEXT,
                tags TEXT,  -- JSON array of tags
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                modified_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Task scheduler table
        execute(R"(
            CREATE TABLE IF NOT EXISTS scheduled_tasks (
                id TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                title TEXT NOT NULL,
                command TEXT NOT NULL,
                schedule TEXT NOT NULL,  -- cron format or natural language
                enabled INTEGER DEFAULT 1,
                last_run DATETIME,
                next_run DATETIME,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Reminders table
        execute(R"(
            CREATE TABLE IF NOT EXISTS reminders (
                id TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                title TEXT NOT NULL,
                message TEXT,
                remind_at DATETIME NOT NULL,
                completed INTEGER DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Environment profiles table
        execute(R"(
            CREATE TABLE IF NOT EXISTS environment_profiles (
                name TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                description TEXT,
                sdk_path TEXT,
                compiler TEXT,
                python_version TEXT,
                node_version TEXT,
                java_version TEXT,
                go_version TEXT,
                rust_version TEXT,
                additional_paths TEXT,  -- JSON array
                variables TEXT,  -- JSON object
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                last_used DATETIME DEFAULT CURRENT_TIMESTAMP,
                UNIQUE(user, name)
            )
        )");

        // Analytics data table
        execute(R"(
            CREATE TABLE IF NOT EXISTS analytics_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                metric_name TEXT NOT NULL,
                value REAL NOT NULL,
                unit TEXT,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                metadata TEXT  -- JSON metadata
            )
        )");

        // P2P sharing metadata table
        execute(R"(
            CREATE TABLE IF NOT EXISTS p2p_shares (
                id TEXT PRIMARY KEY,
                user TEXT NOT NULL,
                filename TEXT NOT NULL,
                filepath TEXT NOT NULL,
                file_size INTEGER,
                checksum TEXT,
                shared_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                expires_at DATETIME,
                download_count INTEGER DEFAULT 0,
                is_public INTEGER DEFAULT 0
            )
        )");

        // P2P downloads table
        execute(R"(
            CREATE TABLE IF NOT EXISTS p2p_downloads (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                share_id TEXT NOT NULL,
                downloader_ip TEXT,
                downloaded_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY(share_id) REFERENCES p2p_shares(id)
            )
        )");
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

bool InternalDB::create_user(const std::string& username, const std::string& password_hash,
                            const std::string& salt, const std::string& role,
                            uint32_t permissions, const std::string& home_directory) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO users (username, password_hash, salt, role, permissions, home_directory)
        VALUES (?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, static_cast<int>(permissions));
    sqlite3_bind_text(stmt, 6, home_directory.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_user(const std::string& username, const std::string& password_hash,
                            const std::string& salt, uint32_t permissions) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        UPDATE users SET password_hash = ?, salt = ?, permissions = ?, updated_at = CURRENT_TIMESTAMP
        WHERE username = ?
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, salt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, static_cast<int>(permissions));
    sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::map<std::string, std::string> InternalDB::get_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT username, password_hash, salt, role, permissions, active, home_directory
        FROM users WHERE username = ?
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::map<std::string, std::string> user_data;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_data["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        user_data["password_hash"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user_data["salt"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        user_data["role"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        user_data["permissions"] = std::to_string(sqlite3_column_int(stmt, 4));
        user_data["active"] = std::to_string(sqlite3_column_int(stmt, 5));
        user_data["home_directory"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    }

    sqlite3_finalize(stmt);
    return user_data;
}

std::vector<std::map<std::string, std::string>> InternalDB::list_users() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT username, role, active, created_at
        FROM users ORDER BY username
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::vector<std::map<std::string, std::string>> users;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> user;
        user["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        user["role"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user["active"] = std::to_string(sqlite3_column_int(stmt, 2));
        user["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
}

bool InternalDB::user_exists(const std::string& username) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

// Vault operations
bool InternalDB::initialize_vault(const std::string& user, const std::string& master_key_hash, const std::string& salt) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT OR REPLACE INTO vault_keys (user, master_key_hash, salt)
        VALUES (?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, master_key_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::is_vault_initialized(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM vault_keys WHERE user = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    bool initialized = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        initialized = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return initialized;
}

std::map<std::string, std::string> InternalDB::get_vault_key(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT master_key_hash, salt FROM vault_keys WHERE user = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::map<std::string, std::string> key_data;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        key_data["master_key_hash"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        key_data["salt"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }

    sqlite3_finalize(stmt);
    return key_data;
}

bool InternalDB::add_vault_password(const std::string& user, const std::string& service,
                                   const std::string& username, const std::string& password,
                                   const std::string& url, const std::string& notes) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT OR REPLACE INTO vault_passwords (user, service, username, password, url, notes)
        VALUES (?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, service.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, notes.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_vault_password(const std::string& user, const std::string& service,
                                      const std::string& username, const std::string& password,
                                      const std::string& url, const std::string& notes) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        UPDATE vault_passwords SET username = ?, password = ?, url = ?, notes = ?, updated_at = CURRENT_TIMESTAMP
        WHERE user = ? AND service = ?
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, service.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_vault_password(const std::string& user, const std::string& service) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM vault_passwords WHERE user = ? AND service = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, service.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::map<std::string, std::string> InternalDB::get_vault_password(const std::string& user, const std::string& service) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT username, password, url, notes FROM vault_passwords WHERE user = ? AND service = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, service.c_str(), -1, SQLITE_TRANSIENT);

    std::map<std::string, std::string> password_data;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        password_data["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        password_data["password"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        password_data["url"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        password_data["notes"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    }

    sqlite3_finalize(stmt);
    return password_data;
}

std::vector<std::map<std::string, std::string>> InternalDB::list_vault_passwords(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT service, username, url, notes, created_at FROM vault_passwords WHERE user = ? ORDER BY service";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> passwords;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> pwd;
        pwd["service"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        pwd["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pwd["url"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        pwd["notes"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        pwd["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        passwords.push_back(pwd);
    }

    sqlite3_finalize(stmt);
    return passwords;
}

std::vector<std::map<std::string, std::string>> InternalDB::search_vault_passwords(const std::string& user, const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT service, username, url, notes
        FROM vault_passwords
        WHERE user = ? AND (service LIKE ? OR username LIKE ? OR url LIKE ? OR notes LIKE ?)
        ORDER BY service
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> pwd;
        pwd["service"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        pwd["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pwd["url"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        pwd["notes"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        results.push_back(pwd);
    }

    sqlite3_finalize(stmt);
    return results;
}

bool InternalDB::clear_vault(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    // Delete all passwords for user
    sqlite3_stmt* stmt1;
    const char* sql1 = "DELETE FROM vault_passwords WHERE user = ?";
    if (sqlite3_prepare_v2(pimpl_->db, sql1, -1, &stmt1, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt1, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt1);
    sqlite3_finalize(stmt1);

    // Delete vault key for user
    sqlite3_stmt* stmt2;
    const char* sql2 = "DELETE FROM vault_keys WHERE user = ?";
    if (sqlite3_prepare_v2(pimpl_->db, sql2, -1, &stmt2, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt2, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt2);
    sqlite3_finalize(stmt2);

    return true;
}

// Notes and Snippets operations
bool InternalDB::add_note(const std::string& user, const std::string& id, const std::string& title,
                         const std::string& content, const std::string& category,
                         const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO notes (id, user, title, content, category, tags)
        VALUES (?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Convert tags vector to JSON string
    std::string tags_json = "[";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) tags_json += ",";
        tags_json += "\"" + tags[i] + "\"";
    }
    tags_json += "]";

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, tags_json.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_note(const std::string& user, const std::string& id, const std::string& title,
                            const std::string& content, const std::string& category,
                            const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        UPDATE notes SET title = ?, content = ?, category = ?, tags = ?, modified_at = CURRENT_TIMESTAMP
        WHERE user = ? AND id = ?
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string tags_json = "[";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) tags_json += ",";
        tags_json += "\"" + tags[i] + "\"";
    }
    tags_json += "]";

    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, tags_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_note(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM notes WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_notes(const std::string& user, const std::string& category) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, title, content, category, tags, created_at, modified_at FROM notes WHERE user = ?";
    if (!category.empty()) {
        sql += " AND category = ?";
    }
    sql += " ORDER BY modified_at DESC";

    if (sqlite3_prepare_v2(pimpl_->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    if (!category.empty()) {
        sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
    }

    std::vector<std::map<std::string, std::string>> notes;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> note;
        note["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        note["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        note["content"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        note["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        note["tags"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        note["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        note["modified_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        notes.push_back(note);
    }

    sqlite3_finalize(stmt);
    return notes;
}

std::vector<std::map<std::string, std::string>> InternalDB::search_notes(const std::string& user, const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, title, content, category, tags, created_at, modified_at
        FROM notes
        WHERE user = ? AND (title LIKE ? OR content LIKE ? OR tags LIKE ?)
        ORDER BY modified_at DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> note;
        note["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        note["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        note["content"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        note["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        note["tags"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        note["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        note["modified_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        results.push_back(note);
    }

    sqlite3_finalize(stmt);
    return results;
}

// Code Snippets operations
bool InternalDB::add_snippet(const std::string& user, const std::string& id, const std::string& title,
                            const std::string& code, const std::string& language,
                            const std::string& description, const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO code_snippets (id, user, title, code, language, description, tags)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string tags_json = "[";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) tags_json += ",";
        tags_json += "\"" + tags[i] + "\"";
    }
    tags_json += "]";

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, language.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, tags_json.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_snippet(const std::string& user, const std::string& id, const std::string& title,
                               const std::string& code, const std::string& language,
                               const std::string& description, const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        UPDATE code_snippets SET title = ?, code = ?, language = ?, description = ?, tags = ?, modified_at = CURRENT_TIMESTAMP
        WHERE user = ? AND id = ?
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string tags_json = "[";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) tags_json += ",";
        tags_json += "\"" + tags[i] + "\"";
    }
    tags_json += "]";

    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, language.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, tags_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_snippet(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM code_snippets WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_snippets(const std::string& user, const std::string& language) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, title, code, language, description, tags, created_at, modified_at FROM code_snippets WHERE user = ?";
    if (!language.empty()) {
        sql += " AND language = ?";
    }
    sql += " ORDER BY modified_at DESC";

    if (sqlite3_prepare_v2(pimpl_->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    if (!language.empty()) {
        sqlite3_bind_text(stmt, 2, language.c_str(), -1, SQLITE_TRANSIENT);
    }

    std::vector<std::map<std::string, std::string>> snippets;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> snippet;
        snippet["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        snippet["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        snippet["code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        snippet["language"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        snippet["description"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        snippet["tags"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        snippet["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        snippet["modified_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        snippets.push_back(snippet);
    }

    sqlite3_finalize(stmt);
    return snippets;
}

std::vector<std::map<std::string, std::string>> InternalDB::search_snippets(const std::string& user, const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, title, code, language, description, tags, created_at, modified_at
        FROM code_snippets
        WHERE user = ? AND (title LIKE ? OR code LIKE ? OR description LIKE ? OR tags LIKE ?)
        ORDER BY modified_at DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> snippet;
        snippet["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        snippet["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        snippet["code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        snippet["language"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        snippet["description"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        snippet["tags"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        snippet["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        snippet["modified_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        results.push_back(snippet);
    }

    sqlite3_finalize(stmt);
    return results;
}

// Task Scheduler operations
bool InternalDB::add_scheduled_task(const std::string& user, const std::string& id, const std::string& title,
                                   const std::string& command, const std::string& schedule) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO scheduled_tasks (id, user, title, command, schedule)
        VALUES (?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, command.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, schedule.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_scheduled_task(const std::string& user, const std::string& id, bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "UPDATE scheduled_tasks SET enabled = ? WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, enabled ? 1 : 0);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_scheduled_task(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM scheduled_tasks WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_scheduled_tasks(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, title, command, schedule, enabled, last_run, next_run, created_at
        FROM scheduled_tasks
        WHERE user = ? ORDER BY created_at DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> tasks;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> task;
        task["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        task["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task["command"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task["schedule"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        task["enabled"] = std::to_string(sqlite3_column_int(stmt, 4));
        task["last_run"] = sqlite3_column_text(stmt, 5) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) : "";
        task["next_run"] = sqlite3_column_text(stmt, 6) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) : "";
        task["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        tasks.push_back(task);
    }

    sqlite3_finalize(stmt);
    return tasks;
}

// Reminder operations
bool InternalDB::add_reminder(const std::string& user, const std::string& id, const std::string& title,
                             const std::string& message, const std::string& remind_at) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO reminders (id, user, title, message, remind_at)
        VALUES (?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, message.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, remind_at.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::complete_reminder(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "UPDATE reminders SET completed = 1 WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_reminder(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM reminders WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_pending_reminders(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, title, message, remind_at, created_at
        FROM reminders
        WHERE user = ? AND completed = 0 AND remind_at <= datetime('now')
        ORDER BY remind_at ASC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> reminders;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> reminder;
        reminder["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        reminder["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        reminder["message"] = sqlite3_column_text(stmt, 2) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "";
        reminder["remind_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        reminder["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        reminders.push_back(reminder);
    }

    sqlite3_finalize(stmt);
    return reminders;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_all_reminders(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT id, title, message, remind_at, completed, created_at
        FROM reminders
        WHERE user = ? ORDER BY remind_at DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> reminders;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> reminder;
        reminder["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        reminder["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        reminder["message"] = sqlite3_column_text(stmt, 2) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "";
        reminder["remind_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        reminder["completed"] = std::to_string(sqlite3_column_int(stmt, 4));
        reminder["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        reminders.push_back(reminder);
    }

    sqlite3_finalize(stmt);
    return reminders;
}

// Environment profiles operations
bool InternalDB::add_environment_profile(const std::string& user, const std::string& name,
                                        const std::string& description, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO environment_profiles (name, user, description, sdk_path, compiler,
                                        python_version, node_version, java_version, go_version, rust_version,
                                        additional_paths, variables)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Create JSON for additional_paths and variables
    std::string additional_paths = "[]";
    std::string variables = "{}";

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, config.count("sdk_path") ? config.at("sdk_path").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, config.count("compiler") ? config.at("compiler").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, config.count("python_version") ? config.at("python_version").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, config.count("node_version") ? config.at("node_version").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, config.count("java_version") ? config.at("java_version").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, config.count("go_version") ? config.at("go_version").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, config.count("rust_version") ? config.at("rust_version").c_str() : nullptr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, additional_paths.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, variables.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::update_environment_profile(const std::string& user, const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "UPDATE environment_profiles SET last_used = CURRENT_TIMESTAMP WHERE user = ? AND name = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::delete_environment_profile(const std::string& user, const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM environment_profiles WHERE user = ? AND name = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_environment_profiles(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT name, description, sdk_path, compiler, python_version, node_version,
               java_version, go_version, rust_version, additional_paths, variables,
               created_at, last_used
        FROM environment_profiles
        WHERE user = ? ORDER BY last_used DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> profiles;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> profile;
        profile["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        profile["description"] = sqlite3_column_text(stmt, 1) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "";
        profile["sdk_path"] = sqlite3_column_text(stmt, 2) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "";
        profile["compiler"] = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        profile["python_version"] = sqlite3_column_text(stmt, 4) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) : "";
        profile["node_version"] = sqlite3_column_text(stmt, 5) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) : "";
        profile["java_version"] = sqlite3_column_text(stmt, 6) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) : "";
        profile["go_version"] = sqlite3_column_text(stmt, 7) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)) : "";
        profile["rust_version"] = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
        profile["additional_paths"] = sqlite3_column_text(stmt, 9) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)) : "";
        profile["variables"] = sqlite3_column_text(stmt, 10) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)) : "";
        profile["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        profile["last_used"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        profiles.push_back(profile);
    }

    sqlite3_finalize(stmt);
    return profiles;
}

// Analytics operations
bool InternalDB::add_analytics_data(const std::string& metric_name, double value,
                                   const std::string& unit, const std::string& metadata) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO analytics_data (metric_name, value, unit, metadata)
        VALUES (?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, metric_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, value);
    sqlite3_bind_text(stmt, 3, unit.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, metadata.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_analytics_data(const std::string& metric_name, int limit) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    std::string sql = "SELECT value, unit, timestamp, metadata FROM analytics_data WHERE metric_name = ? ORDER BY timestamp DESC";
    if (limit > 0) {
        sql += " LIMIT " + std::to_string(limit);
    }

    if (sqlite3_prepare_v2(pimpl_->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, metric_name.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> data;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> point;
        point["value"] = std::to_string(sqlite3_column_double(stmt, 0));
        point["unit"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        point["timestamp"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        point["metadata"] = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        data.push_back(point);
    }

    sqlite3_finalize(stmt);
    return data;
}

std::map<std::string, int> InternalDB::get_analytics_summary() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT metric_name, COUNT(*) as count, AVG(value) as avg_value
        FROM analytics_data
        GROUP BY metric_name
        ORDER BY count DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::map<std::string, int> summary;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string metric = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int count = sqlite3_column_int(stmt, 1);
        summary[metric] = count;
    }

    sqlite3_finalize(stmt);
    return summary;
}

// P2P operations
bool InternalDB::add_p2p_share(const std::string& user, const std::string& share_id,
                              const std::string& filename, const std::string& filepath,
                              long long file_size, const std::string& checksum, bool is_public) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO p2p_shares (id, user, filename, filepath, file_size, checksum, is_public)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, share_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, filename.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, filepath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, file_size);
    sqlite3_bind_text(stmt, 6, checksum.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, is_public ? 1 : 0);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool InternalDB::record_p2p_download(const std::string& share_id, const std::string& downloader_ip) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO p2p_downloads (share_id, downloader_ip) VALUES (?, ?)";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, share_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, downloader_ip.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::vector<std::map<std::string, std::string>> InternalDB::get_p2p_shares(const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT s.id, s.filename, s.filepath, s.file_size, s.checksum, s.shared_at,
               s.expires_at, s.download_count, s.is_public
        FROM p2p_shares s
        WHERE s.user = ? ORDER BY s.shared_at DESC
    )";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<std::map<std::string, std::string>> shares;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> share;
        share["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        share["filename"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        share["filepath"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        share["file_size"] = std::to_string(sqlite3_column_int64(stmt, 3));
        share["checksum"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        share["shared_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        share["expires_at"] = sqlite3_column_text(stmt, 6) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) : "";
        share["download_count"] = std::to_string(sqlite3_column_int(stmt, 7));
        share["is_public"] = std::to_string(sqlite3_column_int(stmt, 8));
        shares.push_back(share);
    }

    sqlite3_finalize(stmt);
    return shares;
}

bool InternalDB::delete_p2p_share(const std::string& user, const std::string& share_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM p2p_shares WHERE user = ? AND id = ?";

    if (sqlite3_prepare_v2(pimpl_->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, share_id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
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
