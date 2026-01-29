#include "vault/password_manager.h"
#include "database/internal_db.h"
#include "auth/authentication.h"
#include <map>
#include <mutex>
#include <random>
#include <algorithm>
#ifdef HAVE_OPENSSL
#include <openssl/evp.h>
#include <openssl/aes.h>
#endif

namespace customos {
namespace vault {

struct PasswordManager::Impl {
    std::map<std::string, PasswordEntry> passwords;
    bool initialized = false;
    bool unlocked = false;
    std::string master_hash;
    std::mutex mutex;
};

PasswordManager::PasswordManager() : pimpl_(std::make_unique<Impl>()) {
}

PasswordManager::~PasswordManager() = default;

PasswordManager& PasswordManager::instance() {
    static PasswordManager instance;
    return instance;
}

bool PasswordManager::initialize(const std::string& master_password) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (pimpl_->initialized) {
        return false;
    }

    // Get current user
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Check if vault is already initialized
    auto& db = database::InternalDB::instance();
    if (db.is_vault_initialized(current_user)) {
        return false; // Already initialized
    }

    // TODO: Properly hash master password with salt
    std::string salt = generate_salt();
    std::string master_hash = hash_password(master_password, salt);

    // Store master key in database
    if (!db.initialize_vault(current_user, master_hash, salt)) {
        return false;
    }

    pimpl_->master_hash = master_hash;
    pimpl_->initialized = true;
    pimpl_->unlocked = true;

    // Load existing passwords from database
    load_passwords_from_database();

    return true;
}

void PasswordManager::load_passwords_from_database() {
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) return;

    auto& db = database::InternalDB::instance();
    auto passwords_data = db.list_vault_passwords(current_user);

    for (const auto& pwd_data : passwords_data) {
        PasswordEntry entry;
        entry.service = pwd_data.at("service");
        entry.username = pwd_data.at("username");
        entry.password = pwd_data.at("password");
        entry.url = pwd_data.at("url");
        entry.notes = pwd_data.at("notes");

        pimpl_->passwords[entry.service] = entry;
    }
}

bool PasswordManager::is_initialized() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->initialized;
}

bool PasswordManager::unlock(const std::string& master_password) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (!pimpl_->initialized) {
        return false;
    }

    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Get vault key from database
    auto& db = database::InternalDB::instance();
    auto vault_key = db.get_vault_key(current_user);

    if (vault_key.empty()) {
        return false;
    }

    // Verify password
    std::string input_hash = hash_password(master_password, vault_key["salt"]);
    if (input_hash == vault_key["master_key_hash"]) {
        pimpl_->unlocked = true;
        pimpl_->master_hash = vault_key["master_key_hash"];

        // Load passwords from database
        load_passwords_from_database();
        return true;
    }

    return false;
}

void PasswordManager::lock() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->unlocked = false;
}

bool PasswordManager::is_unlocked() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->unlocked;
}

bool PasswordManager::change_master_password(const std::string& old_pass, const std::string& new_pass) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Get current vault key from database
    auto& db = database::InternalDB::instance();
    auto vault_key = db.get_vault_key(current_user);

    if (vault_key.empty()) {
        return false;
    }

    // Verify old password
    std::string old_hash = hash_password(old_pass, vault_key["salt"]);
    if (old_hash != vault_key["master_key_hash"]) {
        return false;
    }

    // Generate new salt and hash for new password
    std::string new_salt = generate_salt();
    std::string new_master_hash = hash_password(new_pass, new_salt);

    // Update in database
    if (!db.initialize_vault(current_user, new_master_hash, new_salt)) {
        return false;
    }

    pimpl_->master_hash = new_master_hash;
    return true;
}

bool PasswordManager::add_password(const PasswordEntry& entry) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (!pimpl_->unlocked) {
        return false;
    }

    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Save to database
    auto& db = database::InternalDB::instance();
    if (!db.add_vault_password(current_user, entry.service, entry.username,
                              entry.password, entry.url, entry.notes)) {
        return false;
    }

    pimpl_->passwords[entry.service] = entry;
    return true;
}

bool PasswordManager::update_password(const std::string& service, const PasswordEntry& entry) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (!pimpl_->unlocked) {
        return false;
    }

    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Update in database
    auto& db = database::InternalDB::instance();
    if (!db.update_vault_password(current_user, service, entry.username,
                                 entry.password, entry.url, entry.notes)) {
        return false;
    }

    pimpl_->passwords[service] = entry;
    return true;
}

bool PasswordManager::delete_password(const std::string& service) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (!pimpl_->unlocked) {
        return false;
    }

    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Delete from database
    auto& db = database::InternalDB::instance();
    if (!db.delete_vault_password(current_user, service)) {
        return false;
    }

    return pimpl_->passwords.erase(service) > 0;
}

std::string PasswordManager::hash_password(const std::string& password, const std::string& salt) {
    std::string salted = password + salt;
#ifdef HAVE_OPENSSL
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.c_str()), salted.length(), hash);

    std::string result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        result += buf;
    }
    return result;
#else
    // Fallback: simple hash when OpenSSL not available (NOT SECURE)
    return password + salt + "_insecure_hash";
#endif
}

std::string PasswordManager::generate_salt() {
#ifdef HAVE_OPENSSL
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    std::string result;
    for (int i = 0; i < 16; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", salt[i]);
        result += buf;
    }
    return result;
#else
    // Fallback: pseudo-random salt when OpenSSL not available (NOT SECURE)
    srand(static_cast<unsigned int>(time(nullptr)));
    char buf[32];
    snprintf(buf, sizeof(buf), "%016x", rand());
    return std::string(buf);
#endif
}

customos::optional<PasswordEntry> PasswordManager::get_password(const std::string& service) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (!pimpl_->unlocked) {
        return customos::nullopt;
    }

    auto it = pimpl_->passwords.find(service);
    if (it != pimpl_->passwords.end()) {
        return it->second;
    }

    return customos::nullopt;
}

std::vector<PasswordEntry> PasswordManager::list_passwords() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<PasswordEntry> result;
    for (const auto& pair : pimpl_->passwords) {
        result.push_back(pair.second);
    }

    return result;
}

std::vector<PasswordEntry> PasswordManager::search_passwords(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<PasswordEntry> result;
    for (const auto& pair : pimpl_->passwords) {
        if (pair.first.find(query) != std::string::npos ||
            pair.second.username.find(query) != std::string::npos) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::string PasswordManager::generate_password(const PasswordGenOptions& options) {
    std::string charset;
    
    if (options.use_lowercase) charset += "abcdefghijklmnopqrstuvwxyz";
    if (options.use_uppercase) charset += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (options.use_digits) charset += "0123456789";
    if (options.use_symbols) charset += "!@#$%^&*()-_=+[]{}|;:,.<>?";

    // Remove excluded characters
    for (char c : options.exclude_chars) {
        size_t pos = charset.find(c);
        if (pos != std::string::npos) {
            charset.erase(pos, 1);
        }
    }

    if (charset.empty()) {
        return "";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.length() - 1);

    std::string password;
    for (size_t i = 0; i < options.length; ++i) {
        password += charset[dis(gen)];
    }

    return password;
}

bool PasswordManager::export_vault(const std::string& filepath, const std::string& encryption_key) {
    // TODO: Implement encrypted vault export
    return false;
}

bool PasswordManager::import_vault(const std::string& filepath, const std::string& encryption_key) {
    // TODO: Implement encrypted vault import
    return false;
}

bool PasswordManager::backup_vault(const std::string& backup_path) {
    // TODO: Implement vault backup
    return false;
}

bool PasswordManager::restore_vault(const std::string& backup_path) {
    // TODO: Implement vault restore
    return false;
}

void PasswordManager::clear_clipboard() {
    // TODO: Implement clipboard clearing
}

} // namespace vault
} // namespace customos
