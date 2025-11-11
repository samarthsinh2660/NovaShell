#ifndef CUSTOMOS_AUTHENTICATION_H
#define CUSTOMOS_AUTHENTICATION_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace customos {
namespace auth {

// User roles
enum class UserRole {
    ADMIN,
    USER,
    GUEST
};

// Permission flags
enum class Permission : uint32_t {
    NONE           = 0,
    VFS_READ       = 1 << 0,
    VFS_WRITE      = 1 << 1,
    VFS_EXECUTE    = 1 << 2,
    NETWORK_SNIFF  = 1 << 3,
    NETWORK_ADMIN  = 1 << 4,
    VAULT_READ     = 1 << 5,
    VAULT_WRITE    = 1 << 6,
    USER_MANAGE    = 1 << 7,
    PLUGIN_LOAD    = 1 << 8,
    SCRIPT_RUN     = 1 << 9,
    SYSTEM_ADMIN   = 1 << 10,
    ALL            = 0xFFFFFFFF
};

inline Permission operator|(Permission a, Permission b) {
    return static_cast<Permission>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Permission operator&(Permission a, Permission b) {
    return static_cast<Permission>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

// User information
struct UserInfo {
    std::string username;
    UserRole role;
    uint32_t permissions;
    bool active;
    std::string home_directory;
};

// Authentication system
class Authentication {
public:
    static Authentication& instance();

    // User management
    bool create_user(const std::string& username, const std::string& password, UserRole role);
    bool delete_user(const std::string& username);
    bool change_password(const std::string& username, const std::string& old_pass, const std::string& new_pass);
    bool user_exists(const std::string& username) const;

    // Authentication
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool is_logged_in() const;

    // Current user
    std::string get_current_user() const;
    UserInfo get_current_user_info() const;

    // Permissions
    bool has_permission(Permission perm) const;
    bool grant_permission(const std::string& username, Permission perm);
    bool revoke_permission(const std::string& username, Permission perm);

    // User listing
    std::vector<UserInfo> list_users() const;

private:
    Authentication();
    ~Authentication();
    Authentication(const Authentication&) = delete;
    Authentication& operator=(const Authentication&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace auth
} // namespace customos

#endif // CUSTOMOS_AUTHENTICATION_H
