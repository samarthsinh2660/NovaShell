#ifndef CUSTOMOS_MOBILE_API_H
#define CUSTOMOS_MOBILE_API_H

#include "network/http_server.h"
#include <string>
#include <memory>

namespace customos {
namespace mobile {

class MobileAPI {
public:
    MobileAPI();
    ~MobileAPI();

    // Initialize mobile API server
    bool initialize(int port = 8080);

    // Start/stop the server
    bool start();
    void stop();
    bool is_running() const;

    // Get server stats
    network::HttpServer::Stats get_stats() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

    // API endpoints
    void setup_endpoints();

    // Authentication endpoints
    void handle_login(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_logout(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_status(const network::HttpRequest& req, network::HttpResponse& resp);

    // Vault endpoints
    void handle_vault_list(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_vault_get(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_vault_add(const network::HttpRequest& req, network::HttpResponse& resp);

    // Command execution endpoints
    void handle_execute_command(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_command_history(const network::HttpRequest& req, network::HttpResponse& resp);

    // System monitoring endpoints
    void handle_system_stats(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_process_list(const network::HttpRequest& req, network::HttpResponse& resp);

    // File system endpoints
    void handle_file_list(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_file_read(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_file_write(const network::HttpRequest& req, network::HttpResponse& resp);

    // Notes endpoints
    void handle_notes_list(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_notes_get(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_notes_add(const network::HttpRequest& req, network::HttpResponse& resp);

    // Analytics endpoints
    void handle_analytics_summary(const network::HttpRequest& req, network::HttpResponse& resp);
    void handle_analytics_insights(const network::HttpRequest& req, network::HttpResponse& resp);

    // WebSocket endpoints for real-time updates
    void handle_websocket_connection(const std::string& message);

    // Utility functions
    bool authenticate_request(const network::HttpRequest& req, std::string& user);
    std::string generate_token(const std::string& user);
    bool validate_token(const std::string& token, std::string& user);
    std::string create_json_response(const std::string& status, const std::string& message = "", const std::string& data = "");
    std::string create_error_response(const std::string& error, int code = 400);
};

} // namespace mobile
} // namespace customos

#endif // CUSTOMOS_MOBILE_API_H
