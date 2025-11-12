#ifndef CUSTOMOS_PASSWORD_MANAGER_H
#define CUSTOMOS_PASSWORD_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "compat/optional.h"  // Compatibility layer for older compilers

namespace customos {
namespace vault {

// Password entry
struct PasswordEntry {
    std::string service;
    std::string username;
    std::string password;
    std::string notes;
    std::string url;
    time_t created;
    time_t modified;
};

// Password generation options
struct PasswordGenOptions {
    size_t length = 16;
    bool use_uppercase = true;
    bool use_lowercase = true;
    bool use_digits = true;
    bool use_symbols = true;
    std::string exclude_chars = "";
};

// Password Manager (Vault)
class PasswordManager {
public:
    static PasswordManager& instance();

    // Vault initialization
    bool initialize(const std::string& master_password);
    bool is_initialized() const;
    bool unlock(const std::string& master_password);
    void lock();
    bool is_unlocked() const;
    bool change_master_password(const std::string& old_pass, const std::string& new_pass);

    // Password operations
    bool add_password(const PasswordEntry& entry);
    bool update_password(const std::string& service, const PasswordEntry& entry);
    bool delete_password(const std::string& service);
    customos::optional<PasswordEntry> get_password(const std::string& service);
    std::vector<PasswordEntry> list_passwords();
    std::vector<PasswordEntry> search_passwords(const std::string& query);

    // Password generation
    std::string generate_password(const PasswordGenOptions& options = PasswordGenOptions());

    // Import/Export
    bool export_vault(const std::string& filepath, const std::string& encryption_key);
    bool import_vault(const std::string& filepath, const std::string& encryption_key);

    // Vault maintenance
    bool backup_vault(const std::string& backup_path);
    bool restore_vault(const std::string& backup_path);
    void clear_clipboard();

private:
    PasswordManager();
    ~PasswordManager();
    PasswordManager(const PasswordManager&) = delete;
    PasswordManager& operator=(const PasswordManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace vault
} // namespace customos

#endif // CUSTOMOS_PASSWORD_MANAGER_H
