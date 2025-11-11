#ifndef CUSTOMOS_DB_MANAGER_H
#define CUSTOMOS_DB_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace database {

// Database types
enum class DBType {
    MYSQL,
    POSTGRESQL,
    SQLITE
};

// Query result
struct QueryResult {
    bool success;
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> columns;
    int affected_rows;
    std::string error_message;
};

// Connection configuration
struct DBConfig {
    DBType type;
    std::string host;
    int port;
    std::string database;
    std::string username;
    std::string password;
    std::string options;
};

// Database Manager (User-facing MySQL/PostgreSQL support)
class DBManager {
public:
    static DBManager& instance();

    // Connection management
    bool connect(const std::string& name, const DBConfig& config);
    bool disconnect(const std::string& name);
    bool is_connected(const std::string& name) const;
    std::vector<std::string> list_connections() const;
    bool switch_connection(const std::string& name);
    std::string get_current_connection() const;

    // Quick connection
    bool connect_mysql(const std::string& name, const std::string& host, int port,
                       const std::string& database, const std::string& user, const std::string& password);
    bool connect_postgres(const std::string& name, const std::string& host, int port,
                          const std::string& database, const std::string& user, const std::string& password);
    bool connect_sqlite(const std::string& name, const std::string& file_path);

    // Query execution
    QueryResult execute(const std::string& query);
    QueryResult execute_on(const std::string& connection_name, const std::string& query);
    
    // Parameterized queries (SQL injection prevention)
    QueryResult execute_prepared(const std::string& query, const std::vector<std::string>& params);

    // Database operations
    std::vector<std::string> list_databases();
    std::vector<std::string> list_tables();
    std::vector<std::string> list_columns(const std::string& table);
    bool create_database(const std::string& db_name);
    bool drop_database(const std::string& db_name);

    // Table operations
    bool create_table(const std::string& table_name, const std::map<std::string, std::string>& columns);
    bool drop_table(const std::string& table_name);
    bool truncate_table(const std::string& table_name);

    // Data operations
    QueryResult select(const std::string& table, const std::string& conditions = "");
    QueryResult insert(const std::string& table, const std::map<std::string, std::string>& data);
    QueryResult update(const std::string& table, const std::map<std::string, std::string>& data, 
                       const std::string& conditions);
    QueryResult delete_from(const std::string& table, const std::string& conditions);

    // Import/Export
    bool export_to_csv(const std::string& table, const std::string& file_path);
    bool import_from_csv(const std::string& table, const std::string& file_path);
    bool export_to_sql(const std::string& file_path);
    bool import_from_sql(const std::string& file_path);

    // Backup/Restore
    bool backup_database(const std::string& backup_path);
    bool restore_database(const std::string& backup_path);

    // Utility functions
    std::string escape_string(const std::string& str);
    QueryResult get_table_info(const std::string& table);
    uint64_t get_table_row_count(const std::string& table);

private:
    DBManager();
    ~DBManager();
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace database
} // namespace customos

#endif // CUSTOMOS_DB_MANAGER_H
