#ifndef CUSTOMOS_HTTP_SERVER_H
#define CUSTOMOS_HTTP_SERVER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace customos {
namespace network {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string query_string;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> query_params;
    std::string remote_ip;
};

struct HttpResponse {
    int status_code = 200;
    std::string status_message = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::string content_type = "text/plain";

    void set_json(const std::string& json_data) {
        content_type = "application/json";
        body = json_data;
        headers["Content-Length"] = std::to_string(body.length());
    }

    void set_html(const std::string& html_data) {
        content_type = "text/html";
        body = html_data;
        headers["Content-Length"] = std::to_string(body.length());
    }
};

using HttpHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    // Server management
    bool start(int port, const std::string& host = "0.0.0.0");
    void stop();
    bool is_running() const;

    // Route registration
    void add_route(const std::string& method, const std::string& path, HttpHandler handler);
    void add_static_route(const std::string& path, const std::string& file_path);
    void add_websocket_route(const std::string& path, std::function<void(const std::string&)> on_message);

    // CORS support
    void enable_cors(const std::string& allowed_origins = "*");

    // SSL/TLS support
    bool enable_ssl(const std::string& cert_file, const std::string& key_file);

    // Middleware
    void add_middleware(std::function<void(HttpRequest&, HttpResponse&)> middleware);

    // Statistics
    struct Stats {
        uint64_t total_requests = 0;
        uint64_t active_connections = 0;
        uint64_t bytes_received = 0;
        uint64_t bytes_sent = 0;
        double uptime_seconds = 0;
    };
    Stats get_stats() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

    void handle_request(const HttpRequest& req, HttpResponse& resp);
    void log_request(const HttpRequest& req, const HttpResponse& resp);
};

} // namespace network
} // namespace customos

#endif // CUSTOMOS_HTTP_SERVER_H
