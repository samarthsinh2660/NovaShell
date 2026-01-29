#include "network/http_server.h"
#include "logging/logger.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

namespace customos {
namespace network {

struct HttpServer::Impl {
    int server_socket = -1;
    int port = 0;
    std::string host;
    std::atomic<bool> running{false};
    std::vector<std::thread> worker_threads;
    std::mutex mutex;

    // Routes
    std::unordered_map<std::string, std::unordered_map<std::string, HttpHandler>> routes;
    std::unordered_map<std::string, std::string> static_routes;
    std::unordered_map<std::string, std::function<void(const std::string&)>> websocket_routes;

    // Middleware
    std::vector<std::function<void(HttpRequest&, HttpResponse&)>> middleware;

    // CORS
    bool cors_enabled = false;
    std::string cors_origins = "*";

    // SSL
    bool ssl_enabled = false;
    std::string ssl_cert_file;
    std::string ssl_key_file;

    // Stats
    std::atomic<uint64_t> total_requests{0};
    std::atomic<uint64_t> active_connections{0};
    std::atomic<uint64_t> bytes_received{0};
    std::atomic<uint64_t> bytes_sent{0};
    std::chrono::steady_clock::time_point start_time;

    // Parent reference
    HttpServer* parent = nullptr;

    // Utility functions
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::unordered_map<std::string, std::string> parse_query_string(const std::string& query) {
        std::unordered_map<std::string, std::string> params;
        if (query.empty()) return params;

        std::vector<std::string> pairs = split(query, '&');
        for (const auto& pair : pairs) {
            size_t equals_pos = pair.find('=');
            if (equals_pos != std::string::npos) {
                std::string key = pair.substr(0, equals_pos);
                std::string value = pair.substr(equals_pos + 1);
                // URL decode (basic implementation)
                std::replace(key.begin(), key.end(), '+', ' ');
                std::replace(value.begin(), value.end(), '+', ' ');
                params[key] = value;
            }
        }
        return params;
    }

    std::string url_decode(const std::string& encoded) {
        std::string result;
        for (size_t i = 0; i < encoded.length(); ++i) {
            if (encoded[i] == '%') {
                if (i + 2 < encoded.length()) {
                    std::string hex = encoded.substr(i + 1, 2);
                    char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
                    result += ch;
                    i += 2;
                }
            } else if (encoded[i] == '+') {
                result += ' ';
            } else {
                result += encoded[i];
            }
        }
        return result;
    }

    HttpRequest parse_request(const std::string& request_data, const std::string& remote_ip) {
        HttpRequest req;
        req.remote_ip = remote_ip;

        std::istringstream iss(request_data);
        std::string line;

        // Parse request line
        if (std::getline(iss, line)) {
            std::vector<std::string> parts = split(line, ' ');
            if (parts.size() >= 2) {
                req.method = parts[0];
                std::string full_path = parts[1];

                // Split path and query string
                size_t query_pos = full_path.find('?');
                if (query_pos != std::string::npos) {
                    req.path = url_decode(full_path.substr(0, query_pos));
                    req.query_string = full_path.substr(query_pos + 1);
                    req.query_params = parse_query_string(req.query_string);
                } else {
                    req.path = url_decode(full_path);
                }
            }
        }

        // Parse headers
        while (std::getline(iss, line) && !line.empty()) {
            if (line.back() == '\r') line.pop_back();
            if (line.empty()) break;

            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 1);
                // Trim whitespace
                key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](int ch) { return !std::isspace(ch); }));
                key.erase(std::find_if(key.rbegin(), key.rend(), [](int ch) { return !std::isspace(ch); }).base(), key.end());
                value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) { return !std::isspace(ch); }));
                value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) { return !std::isspace(ch); }).base(), value.end());
                req.headers[key] = value;
            }
        }

        // Parse body if present
        if (req.headers.count("Content-Length")) {
            size_t content_length = std::stoul(req.headers["Content-Length"]);
            if (content_length > 0) {
                req.body.resize(content_length);
                iss.read(&req.body[0], content_length);
            }
        }

        return req;
    }

    std::string generate_response(HttpResponse& resp) {
        std::ostringstream oss;

        // Status line
        oss << "HTTP/1.1 " << resp.status_code << " " << resp.status_message << "\r\n";

        // Headers
        resp.headers["Content-Type"] = resp.content_type;
        if (!resp.body.empty() && !resp.headers.count("Content-Length")) {
            resp.headers["Content-Length"] = std::to_string(resp.body.length());
        }

        // CORS headers
        if (cors_enabled) {
            oss << "Access-Control-Allow-Origin: " << cors_origins << "\r\n";
            oss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
            oss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        }

        for (const auto& header : resp.headers) {
            oss << header.first << ": " << header.second << "\r\n";
        }

        oss << "\r\n";
        oss << resp.body;

        return oss.str();
    }

    void handle_client(int client_socket, const std::string& remote_ip) {
        active_connections++;
        total_requests++;

        try {
            char buffer[8192];
            std::string request_data;

            // Read request
            ssize_t bytes_read;
            while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[bytes_read] = '\0';
                request_data += buffer;
                bytes_received += bytes_read;

                // Check if we have a complete HTTP request
                if (request_data.find("\r\n\r\n") != std::string::npos) {
                    break;
                }
            }

            if (!request_data.empty()) {
                // Parse request
                HttpRequest req = parse_request(request_data, remote_ip);

                // Apply middleware
                HttpResponse resp;
                for (auto& mw : middleware) {
                    mw(req, resp);
                }

                // Handle request
                parent->handle_request(req, resp);

                // Send response
                std::string response_str = generate_response(resp);
                send(client_socket, response_str.c_str(), response_str.length(), 0);
                bytes_sent += response_str.length();

                // Log request
                parent->log_request(req, resp);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("HTTP server error handling client: " + std::string(e.what()));
        }

        active_connections--;

#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
    }

    void worker_thread() {
        while (running) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
            if (client_socket >= 0) {
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

                std::thread client_thread(&Impl::handle_client, this, client_socket, std::string(client_ip));
                client_thread.detach();
            }
        }
    }
};

HttpServer::HttpServer() : pimpl_(std::make_unique<Impl>()) {
    pimpl_->parent = this;
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port, const std::string& host) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    if (pimpl_->running) {
        return false;
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG_ERROR("Failed to initialize Winsock");
        return false;
    }
#endif

    pimpl_->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (pimpl_->server_socket < 0) {
        LOG_ERROR("Failed to create socket");
        return false;
    }

    // Allow address reuse
    int opt = 1;
    setsockopt(pimpl_->server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (bind(pimpl_->server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        LOG_ERROR("Failed to bind socket");
#ifdef _WIN32
        closesocket(pimpl_->server_socket);
#else
        close(pimpl_->server_socket);
#endif
        return false;
    }

    if (listen(pimpl_->server_socket, 10) < 0) {
        LOG_ERROR("Failed to listen on socket");
#ifdef _WIN32
        closesocket(pimpl_->server_socket);
#else
        close(pimpl_->server_socket);
#endif
        return false;
    }

    pimpl_->running = true;
    pimpl_->port = port;
    pimpl_->host = host;
    pimpl_->start_time = std::chrono::steady_clock::now();

    // Start worker threads
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    for (unsigned int i = 0; i < num_threads; ++i) {
        pimpl_->worker_threads.emplace_back(&Impl::worker_thread, pimpl_.get());
    }

    LOG_INFO("HTTP server started on " + host + ":" + std::to_string(port));
    return true;
}

void HttpServer::stop() {
    if (!pimpl_->running) return;

    pimpl_->running = false;

#ifdef _WIN32
    closesocket(pimpl_->server_socket);
#else
    close(pimpl_->server_socket);
#endif

    for (auto& thread : pimpl_->worker_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    pimpl_->worker_threads.clear();
    LOG_INFO("HTTP server stopped");
}

bool HttpServer::is_running() const {
    return pimpl_->running;
}

void HttpServer::add_route(const std::string& method, const std::string& path, HttpHandler handler) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->routes[method][path] = handler;
}

void HttpServer::add_static_route(const std::string& path, const std::string& file_path) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->static_routes[path] = file_path;
}

void HttpServer::add_websocket_route(const std::string& path, std::function<void(const std::string&)> on_message) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->websocket_routes[path] = on_message;
}

void HttpServer::enable_cors(const std::string& allowed_origins) {
    pimpl_->cors_enabled = true;
    pimpl_->cors_origins = allowed_origins;
}

bool HttpServer::enable_ssl(const std::string& cert_file, const std::string& key_file) {
    // SSL implementation would go here
    // For now, just store the file paths
    pimpl_->ssl_enabled = true;
    pimpl_->ssl_cert_file = cert_file;
    pimpl_->ssl_key_file = key_file;
    return true;
}

void HttpServer::add_middleware(std::function<void(HttpRequest&, HttpResponse&)> middleware) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->middleware.push_back(middleware);
}

HttpServer::Stats HttpServer::get_stats() const {
    Stats stats;
    stats.total_requests = pimpl_->total_requests;
    stats.active_connections = pimpl_->active_connections;
    stats.bytes_received = pimpl_->bytes_received;
    stats.bytes_sent = pimpl_->bytes_sent;

    auto now = std::chrono::steady_clock::now();
    auto duration = now - pimpl_->start_time;
    stats.uptime_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    return stats;
}

void HttpServer::handle_request(const HttpRequest& req, HttpResponse& resp) {
    // Handle CORS preflight
    if (req.method == "OPTIONS") {
        resp.status_code = 200;
        return;
    }

    // Check static routes first
    if (pimpl_->static_routes.count(req.path)) {
        // Serve static file
        std::ifstream file(pimpl_->static_routes[req.path], std::ios::binary);
        if (file) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            resp.body = content;
            resp.content_type = "text/html"; // Basic implementation
        } else {
            resp.status_code = 404;
            resp.status_message = "Not Found";
        }
        return;
    }

    // Check dynamic routes
    if (pimpl_->routes.count(req.method) && pimpl_->routes[req.method].count(req.path)) {
        try {
            pimpl_->routes[req.method][req.path](req, resp);
        } catch (const std::exception& e) {
            resp.status_code = 500;
            resp.status_message = "Internal Server Error";
            resp.body = "Server error: " + std::string(e.what());
        }
        return;
    }

    // 404 Not Found
    resp.status_code = 404;
    resp.status_message = "Not Found";
    resp.set_json("{\"error\": \"Endpoint not found\"}");
}

void HttpServer::log_request(const HttpRequest& req, const HttpResponse& resp) {
    LOG_INFO("HTTP " + req.method + " " + req.path + " " + std::to_string(resp.status_code) + " from " + req.remote_ip);
}

} // namespace network
} // namespace customos
