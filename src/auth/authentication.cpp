#include "auth/authentication.h"
#include <map>
#include <mutex>
#include <openssl/sha.h>
#include <openssl/rand.h>

namespace customos {
namespace auth {

struct UserData {
    std::string username;
    std::string password_hash;
    std::string salt;
    UserRole role;
    uint32_t permissions;
    bool active;
    std::string home_directory;
};

struct Authentication::Impl {
    std::map<std::string, UserData> users;
    std::string current_user;
    bool logged_in = false;
    std::mutex mutex;

    std::string hash_password(const std::string& password, const std::string& salt) {
        std::string salted = password + salt;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(salted.c_str()), salted.length(), hash);
        
        std::string result;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", hash[i]);
            result += buf;
        }
        return result;
    }

    std::string generate_salt() {
        unsigned char salt[16];
        RAND_bytes(salt, sizeof(salt));
        
        std::string result;
        for (int i = 0; i < 16; i++) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", salt[i]);
            result += buf;
        }
        return result;
    }
};

Authentication::Authentication() : pimpl_(std::make_unique<Impl>()) {
    // Create default admin user
    create_user("admin", "admin", UserRole::ADMIN);
}

Authentication::~Authentication() = default;

Authentication& Authentication::instance() {
    static Authentication instance;
    return instance;
}

bool Authentication::create_user(const std::string& username, const std::string& password, UserRole role) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (pimpl_->users.find(username) != pimpl_->users.end()) {
        return false;
    }

    UserData user;
    user.username = username;
    user.salt = pimpl_->generate_salt();
    user.password_hash = pimpl_->hash_password(password, user.salt);
    user.role = role;
    user.active = true;
    user.home_directory = "/" + username;
    
    // Set permissions based on role
    if (role == UserRole::ADMIN) {
        user.permissions = static_cast<uint32_t>(Permission::ALL);
    } else if (role == UserRole::USER) {
        user.permissions = static_cast<uint32_t>(Permission::VFS_READ | Permission::VFS_WRITE | 
                                                  Permission::SCRIPT_RUN | Permission::VAULT_READ | Permission::VAULT_WRITE);
    } else {
        user.permissions = static_cast<uint32_t>(Permission::VFS_READ);
    }

    pimpl_->users[username] = user;
    return true;
}

bool Authentication::delete_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->users.erase(username) > 0;
}

bool Authentication::change_password(const std::string& username, const std::string& old_pass, const std::string& new_pass) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    auto it = pimpl_->users.find(username);
    if (it == pimpl_->users.end()) {
        return false;
    }

    std::string old_hash = pimpl_->hash_password(old_pass, it->second.salt);
    if (old_hash != it->second.password_hash) {
        return false;
    }

    it->second.salt = pimpl_->generate_salt();
    it->second.password_hash = pimpl_->hash_password(new_pass, it->second.salt);
    return true;
}

bool Authentication::user_exists(const std::string& username) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->users.find(username) != pimpl_->users.end();
}

bool Authentication::login(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    auto it = pimpl_->users.find(username);
    if (it == pimpl_->users.end() || !it->second.active) {
        return false;
    }

    std::string hash = pimpl_->hash_password(password, it->second.salt);
    if (hash == it->second.password_hash) {
        pimpl_->current_user = username;
        pimpl_->logged_in = true;
        return true;
    }

    return false;
}

void Authentication::logout() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->current_user.clear();
    pimpl_->logged_in = false;
}

bool Authentication::is_logged_in() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->logged_in;
}

std::string Authentication::get_current_user() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->current_user;
}

UserInfo Authentication::get_current_user_info() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->current_user.empty()) {
        return UserInfo{};
    }

    auto it = pimpl_->users.find(pimpl_->current_user);
    if (it != pimpl_->users.end()) {
        UserInfo info;
        info.username = it->second.username;
        info.role = it->second.role;
        info.permissions = it->second.permissions;
        info.active = it->second.active;
        info.home_directory = it->second.home_directory;
        return info;
    }

    return UserInfo{};
}

bool Authentication::has_permission(Permission perm) const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (!pimpl_->logged_in) {
        return false;
    }

    auto it = pimpl_->users.find(pimpl_->current_user);
    if (it != pimpl_->users.end()) {
        return (it->second.permissions & static_cast<uint32_t>(perm)) != 0;
    }

    return false;
}

bool Authentication::grant_permission(const std::string& username, Permission perm) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->users.find(username);
    if (it != pimpl_->users.end()) {
        it->second.permissions |= static_cast<uint32_t>(perm);
        return true;
    }

    return false;
}

bool Authentication::revoke_permission(const std::string& username, Permission perm) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->users.find(username);
    if (it != pimpl_->users.end()) {
        it->second.permissions &= ~static_cast<uint32_t>(perm);
        return true;
    }

    return false;
}

std::vector<UserInfo> Authentication::list_users() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<UserInfo> users;
    for (const auto& pair : pimpl_->users) {
        UserInfo info;
        info.username = pair.second.username;
        info.role = pair.second.role;
        info.permissions = pair.second.permissions;
        info.active = pair.second.active;
        info.home_directory = pair.second.home_directory;
        users.push_back(info);
    }

    return users;
}

} // namespace auth
} // namespace customos
