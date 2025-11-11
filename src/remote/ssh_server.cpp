#include "remote/ssh_server.h"
#include <vector>
#include <mutex>

namespace customos {
namespace remote {

struct SSHServer::Impl {
    bool running = false;
    uint16_t port = 2222;
    int max_connections = 10;
    bool password_auth = true;
    bool key_auth = true;
    int idle_timeout = 300;
    std::vector<SSHConnection> connections;
    AuthCallback auth_callback;
    std::mutex mutex;
};

SSHServer::SSHServer() : pimpl_(std::make_unique<Impl>()) {
}

SSHServer::~SSHServer() {
    stop();
}

SSHServer& SSHServer::instance() {
    static SSHServer instance;
    return instance;
}

bool SSHServer::start(uint16_t port) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->running) {
        return false;
    }

    pimpl_->port = port;
    // TODO: Implement actual SSH server using libssh
    pimpl_->running = true;
    
    return true;
}

void SSHServer::stop() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->running = false;
    pimpl_->connections.clear();
}

bool SSHServer::is_running() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->running;
}

void SSHServer::set_host_key(const std::string& key_path) {
    // TODO: Load host key
}

void SSHServer::set_authorized_keys(const std::string& keys_path) {
    // TODO: Load authorized keys
}

void SSHServer::set_max_connections(int max_conn) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->max_connections = max_conn;
}

std::vector<SSHConnection> SSHServer::get_active_connections() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->connections;
}

bool SSHServer::disconnect_client(const std::string& client_ip) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    // TODO: Implement disconnect
    return false;
}

void SSHServer::enable_password_auth(bool enable) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->password_auth = enable;
}

void SSHServer::enable_key_auth(bool enable) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->key_auth = enable;
}

void SSHServer::set_idle_timeout(int seconds) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->idle_timeout = seconds;
}

void SSHServer::set_auth_callback(AuthCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->auth_callback = callback;
}

} // namespace remote
} // namespace customos
