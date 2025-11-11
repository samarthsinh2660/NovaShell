#ifndef CUSTOMOS_SSH_SERVER_H
#define CUSTOMOS_SSH_SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace customos {
namespace remote {

// SSH connection information
struct SSHConnection {
    std::string client_ip;
    uint16_t client_port;
    std::string username;
    time_t connected_at;
    bool authenticated;
};

// SSH Server for remote shell access
class SSHServer {
public:
    static SSHServer& instance();

    // Server control
    bool start(uint16_t port = 2222);
    void stop();
    bool is_running() const;

    // Configuration
    void set_host_key(const std::string& key_path);
    void set_authorized_keys(const std::string& keys_path);
    void set_max_connections(int max_conn);

    // Connection management
    std::vector<SSHConnection> get_active_connections() const;
    bool disconnect_client(const std::string& client_ip);

    // Security settings
    void enable_password_auth(bool enable);
    void enable_key_auth(bool enable);
    void set_idle_timeout(int seconds);

    // Callbacks
    using AuthCallback = std::function<bool(const std::string& username, const std::string& password)>;
    void set_auth_callback(AuthCallback callback);

private:
    SSHServer();
    ~SSHServer();
    SSHServer(const SSHServer&) = delete;
    SSHServer& operator=(const SSHServer&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace remote
} // namespace customos

#endif // CUSTOMOS_SSH_SERVER_H
