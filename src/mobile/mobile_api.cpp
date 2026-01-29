#include "mobile/mobile_api.h"
#include "network/http_server.h"
#include "core/command_processor.h"
#include "auth/authentication.h"
#include "vault/password_manager.h"
#include "database/internal_db.h"
#include "monitor/system_monitor.h"
#include "vfs/virtual_file_system.h"
#include "notes/snippet_manager.h"
#include "logging/logger.h"
#include <nlohmann/json.hpp>
#include <jwt-cpp/jwt.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace customos {
namespace mobile {

struct MobileAPI::Impl {
    network::HttpServer server;
    std::string jwt_secret;
    std::unordered_map<std::string, std::string> active_sessions; // token -> user

    Impl() {
        // Generate a random JWT secret
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        std::stringstream ss;
        for (int i = 0; i < 32; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
        }
        jwt_secret = ss.str();
    }

    std::string create_jwt(const std::string& user) {
        auto token = jwt::create()
            .set_issuer("novashell")
            .set_subject(user)
            .set_payload_claim("user", jwt::claim(std::string(user)))
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
            .sign(jwt::algorithm::hs256{jwt_secret});

        active_sessions[token] = user;
        return token;
    }

    bool validate_jwt(const std::string& token, std::string& user) {
        try {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                .with_issuer("novashell")
                .allow_algorithm(jwt::algorithm::hs256{jwt_secret});

            verifier.verify(decoded);

            user = decoded.get_payload_claim("user").as_string();
            return active_sessions.count(token) && active_sessions[token] == user;
        } catch (...) {
            return false;
        }
    }

    std::string get_auth_token(const network::HttpRequest& req) {
        auto auth_header = req.headers.find("Authorization");
        if (auth_header != req.headers.end()) {
            std::string header = auth_header->second;
            if (header.find("Bearer ") == 0) {
                return header.substr(7); // Remove "Bearer " prefix
            }
        }
        return "";
    }

    nlohmann::json create_success_response(const std::string& message = "", const nlohmann::json& data = nullptr) {
        nlohmann::json response = {
            {"success", true},
            {"message", message}
        };
        if (!data.is_null()) {
            response["data"] = data;
        }
        return response;
    }

    nlohmann::json create_error_response(const std::string& error, int code = 400) {
        return {
            {"success", false},
            {"error", error},
            {"code", code}
        };
    }
};

MobileAPI::MobileAPI() : pimpl_(std::make_unique<Impl>()) {
}

MobileAPI::~MobileAPI() {
    stop();
}

bool MobileAPI::initialize(int port) {
    setup_endpoints();

    // Enable CORS for mobile apps
    pimpl_->server.enable_cors();

    // Add authentication middleware
    pimpl_->server.add_middleware([this](network::HttpRequest& req, network::HttpResponse& resp) {
        // Skip auth for login endpoint
        if (req.path == "/api/auth/login") {
            return;
        }

        std::string token = pimpl_->get_auth_token(req);
        std::string user;
        if (!pimpl_->validate_jwt(token, user)) {
            resp.status_code = 401;
            resp.set_json(pimpl_->create_error_response("Unauthorized", 401).dump());
            return;
        }

        // Add user to request for handlers
        req.headers["X-User"] = user;
    });

    return pimpl_->server.start(port);
}

bool MobileAPI::start() {
    // Server is already started in initialize
    return is_running();
}

void MobileAPI::stop() {
    pimpl_->server.stop();
}

bool MobileAPI::is_running() const {
    return pimpl_->server.is_running();
}

network::HttpServer::Stats MobileAPI::get_stats() const {
    return pimpl_->server.get_stats();
}

void MobileAPI::setup_endpoints() {
    // Authentication
    pimpl_->server.add_route("POST", "/api/auth/login", [this](const auto& req, auto& resp) {
        handle_login(req, resp);
    });
    pimpl_->server.add_route("POST", "/api/auth/logout", [this](const auto& req, auto& resp) {
        handle_logout(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/auth/status", [this](const auto& req, auto& resp) {
        handle_status(req, resp);
    });

    // Vault
    pimpl_->server.add_route("GET", "/api/vault/list", [this](const auto& req, auto& resp) {
        handle_vault_list(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/vault/get/{service}", [this](const auto& req, auto& resp) {
        handle_vault_get(req, resp);
    });
    pimpl_->server.add_route("POST", "/api/vault/add", [this](const auto& req, auto& resp) {
        handle_vault_add(req, resp);
    });

    // Command execution
    pimpl_->server.add_route("POST", "/api/command/execute", [this](const auto& req, auto& resp) {
        handle_execute_command(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/command/history", [this](const auto& req, auto& resp) {
        handle_command_history(req, resp);
    });

    // System monitoring
    pimpl_->server.add_route("GET", "/api/system/stats", [this](const auto& req, auto& resp) {
        handle_system_stats(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/system/processes", [this](const auto& req, auto& resp) {
        handle_process_list(req, resp);
    });

    // File system
    pimpl_->server.add_route("GET", "/api/files/list", [this](const auto& req, auto& resp) {
        handle_file_list(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/files/read", [this](const auto& req, auto& resp) {
        handle_file_read(req, resp);
    });
    pimpl_->server.add_route("POST", "/api/files/write", [this](const auto& req, auto& resp) {
        handle_file_write(req, resp);
    });

    // Notes
    pimpl_->server.add_route("GET", "/api/notes/list", [this](const auto& req, auto& resp) {
        handle_notes_list(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/notes/get/{id}", [this](const auto& req, auto& resp) {
        handle_notes_get(req, resp);
    });
    pimpl_->server.add_route("POST", "/api/notes/add", [this](const auto& req, auto& resp) {
        handle_notes_add(req, resp);
    });

    // Analytics
    pimpl_->server.add_route("GET", "/api/analytics/summary", [this](const auto& req, auto& resp) {
        handle_analytics_summary(req, resp);
    });
    pimpl_->server.add_route("GET", "/api/analytics/insights", [this](const auto& req, auto& resp) {
        handle_analytics_insights(req, resp);
    });

    // WebSocket for real-time updates
    pimpl_->server.add_websocket_route("/ws/updates", [this](const std::string& message) {
        handle_websocket_connection(message);
    });
}

// Authentication endpoints
void MobileAPI::handle_login(const network::HttpRequest& req, network::HttpResponse& resp) {
    try {
        nlohmann::json body = nlohmann::json::parse(req.body);

        std::string username = body["username"];
        std::string password = body["password"];

        auto& auth = auth::Authentication::instance();
        if (auth.login(username, password)) {
            std::string token = pimpl_->create_jwt(username);
            nlohmann::json response = pimpl_->create_success_response("Login successful");
            response["data"] = {
                {"token", token},
                {"user", username}
            };
            resp.set_json(response.dump());
        } else {
            resp.status_code = 401;
            resp.set_json(pimpl_->create_error_response("Invalid credentials", 401).dump());
        }
    } catch (...) {
        resp.status_code = 400;
        resp.set_json(pimpl_->create_error_response("Invalid request format").dump());
    }
}

void MobileAPI::handle_logout(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string token = pimpl_->get_auth_token(req);
    if (!token.empty()) {
        pimpl_->active_sessions.erase(token);
    }
    resp.set_json(pimpl_->create_success_response("Logged out").dump());
}

void MobileAPI::handle_status(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");
    nlohmann::json response = pimpl_->create_success_response("Authenticated");
    response["data"] = {
        {"user", user},
        {"authenticated", true}
    };
    resp.set_json(response.dump());
}

// Vault endpoints
void MobileAPI::handle_vault_list(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");

    auto& vault = vault::PasswordManager::instance();
    if (!vault.is_unlocked()) {
        resp.status_code = 403;
        resp.set_json(pimpl_->create_error_response("Vault is locked", 403).dump());
        return;
    }

    // Get vault entries from database
    auto& db = database::InternalDB::instance();
    auto entries = db.get_vault_passwords(user);

    nlohmann::json entry_list = nlohmann::json::array();
    for (const auto& entry : entries) {
        entry_list.push_back({
            {"service", entry.at("service")},
            {"username", entry.at("username")},
            {"url", entry.at("url")},
            {"notes", entry.at("notes")}
        });
    }

    nlohmann::json response = pimpl_->create_success_response("Vault entries retrieved");
    response["data"] = entry_list;
    resp.set_json(response.dump());
}

void MobileAPI::handle_vault_get(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");
    std::string service = req.query_params["service"];

    auto& vault = vault::PasswordManager::instance();
    if (!vault.is_unlocked()) {
        resp.status_code = 403;
        resp.set_json(pimpl_->create_error_response("Vault is locked", 403).dump());
        return;
    }

    auto& db = database::InternalDB::instance();
    auto entry = db.get_vault_password(user, service);

    if (entry.empty()) {
        resp.status_code = 404;
        resp.set_json(pimpl_->create_error_response("Entry not found", 404).dump());
        return;
    }

    nlohmann::json response = pimpl_->create_success_response("Entry retrieved");
    response["data"] = {
        {"service", entry.at("service")},
        {"username", entry.at("username")},
        {"password", entry.at("password")},
        {"url", entry.at("url")},
        {"notes", entry.at("notes")}
    };
    resp.set_json(response.dump());
}

void MobileAPI::handle_vault_add(const network::HttpRequest& req, network::HttpResponse& resp) {
    try {
        std::string user = req.headers.at("X-User");
        nlohmann::json body = nlohmann::json::parse(req.body);

        vault::PasswordEntry entry;
        entry.service = body["service"];
        entry.username = body["username"];
        entry.password = body["password"];
        entry.url = body.value("url", "");
        entry.notes = body.value("notes", "");

        auto& vault = vault::PasswordManager::instance();
        if (!vault.is_unlocked()) {
            resp.status_code = 403;
            resp.set_json(pimpl_->create_error_response("Vault is locked", 403).dump());
            return;
        }

        if (vault.add_password(entry)) {
            resp.set_json(pimpl_->create_success_response("Password added successfully").dump());
        } else {
            resp.status_code = 400;
            resp.set_json(pimpl_->create_error_response("Failed to add password").dump());
        }
    } catch (...) {
        resp.status_code = 400;
        resp.set_json(pimpl_->create_error_response("Invalid request format").dump());
    }
}

// Command execution endpoints
void MobileAPI::handle_execute_command(const network::HttpRequest& req, network::HttpResponse& resp) {
    try {
        std::string user = req.headers.at("X-User");
        nlohmann::json body = nlohmann::json::parse(req.body);
        std::string command = body["command"];

        // Execute command
        auto& processor = core::CommandProcessor::instance();
        auto result = processor.process(command);

        nlohmann::json response = pimpl_->create_success_response("Command executed");
        response["data"] = {
            {"command", command},
            {"output", result.output},
            {"success", result.success},
            {"exit_code", result.exit_code}
        };
        resp.set_json(response.dump());
    } catch (...) {
        resp.status_code = 400;
        resp.set_json(pimpl_->create_error_response("Invalid request format").dump());
    }
}

void MobileAPI::handle_command_history(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");

    auto& db = database::InternalDB::instance();
    auto history = db.get_history(20); // Last 20 commands

    nlohmann::json history_list = nlohmann::json::array();
    for (const auto& cmd : history) {
        history_list.push_back(cmd);
    }

    nlohmann::json response = pimpl_->create_success_response("Command history retrieved");
    response["data"] = history_list;
    resp.set_json(response.dump());
}

// System monitoring endpoints
void MobileAPI::handle_system_stats(const network::HttpRequest& req, network::HttpResponse& resp) {
    auto& monitor = monitor::SystemMonitor::instance();
    auto stats = monitor.get_current_stats();

    nlohmann::json response = pimpl_->create_success_response("System stats retrieved");
    response["data"] = {
        {"cpu_usage", stats.cpu_usage},
        {"memory_usage", stats.memory_usage},
        {"disk_usage", stats.disk_usage},
        {"network_rx", stats.network_rx},
        {"network_tx", stats.network_tx},
        {"uptime", stats.uptime_seconds}
    };
    resp.set_json(response.dump());
}

void MobileAPI::handle_process_list(const network::HttpRequest& req, network::HttpResponse& resp) {
    auto& monitor = monitor::SystemMonitor::instance();
    auto processes = monitor.get_process_list(10); // Top 10 processes

    nlohmann::json process_list = nlohmann::json::array();
    for (const auto& proc : processes) {
        process_list.push_back({
            {"pid", proc.pid},
            {"name", proc.name},
            {"cpu_usage", proc.cpu_usage},
            {"memory_usage", proc.memory_usage},
            {"status", proc.status}
        });
    }

    nlohmann::json response = pimpl_->create_success_response("Process list retrieved");
    response["data"] = process_list;
    resp.set_json(response.dump());
}

// File system endpoints
void MobileAPI::handle_file_list(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string path = req.query_params.count("path") ? req.query_params.at("path") : "/";

    auto& vfs = vfs::VirtualFileSystem::instance();
    auto entries = vfs.list_directory(path);

    nlohmann::json file_list = nlohmann::json::array();
    for (const auto& entry : entries) {
        file_list.push_back({
            {"name", entry.name},
            {"type", entry.is_directory ? "directory" : "file"},
            {"size", entry.size},
            {"modified", entry.modified_time}
        });
    }

    nlohmann::json response = pimpl_->create_success_response("File list retrieved");
    response["data"] = file_list;
    resp.set_json(response.dump());
}

void MobileAPI::handle_file_read(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string path = req.query_params.at("path");

    auto& vfs = vfs::VirtualFileSystem::instance();
    std::string content = vfs.read_file(path);

    nlohmann::json response = pimpl_->create_success_response("File content retrieved");
    response["data"] = {
        {"path", path},
        {"content", content}
    };
    resp.set_json(response.dump());
}

void MobileAPI::handle_file_write(const network::HttpRequest& req, network::HttpResponse& resp) {
    try {
        nlohmann::json body = nlohmann::json::parse(req.body);
        std::string path = body["path"];
        std::string content = body["content"];

        auto& vfs = vfs::VirtualFileSystem::instance();
        if (vfs.write_file(path, content)) {
            resp.set_json(pimpl_->create_success_response("File written successfully").dump());
        } else {
            resp.status_code = 500;
            resp.set_json(pimpl_->create_error_response("Failed to write file").dump());
        }
    } catch (...) {
        resp.status_code = 400;
        resp.set_json(pimpl_->create_error_response("Invalid request format").dump());
    }
}

// Notes endpoints
void MobileAPI::handle_notes_list(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");
    std::string category = req.query_params.count("category") ? req.query_params.at("category") : "";

    auto& db = database::InternalDB::instance();
    auto notes = db.get_notes(user, category);

    nlohmann::json notes_list = nlohmann::json::array();
    for (const auto& note : notes) {
        notes_list.push_back({
            {"id", note.at("id")},
            {"title", note.at("title")},
            {"category", note.at("category")},
            {"created_at", note.at("created_at")},
            {"modified_at", note.at("modified_at")}
        });
    }

    nlohmann::json response = pimpl_->create_success_response("Notes retrieved");
    response["data"] = notes_list;
    resp.set_json(response.dump());
}

void MobileAPI::handle_notes_get(const network::HttpRequest& req, network::HttpResponse& resp) {
    std::string user = req.headers.at("X-User");
    std::string id = req.query_params.at("id");

    auto& db = database::InternalDB::instance();
    auto notes = db.get_notes(user);

    for (const auto& note : notes) {
        if (note.at("id") == id) {
            nlohmann::json response = pimpl_->create_success_response("Note retrieved");
            response["data"] = {
                {"id", note.at("id")},
                {"title", note.at("title")},
                {"content", note.at("content")},
                {"category", note.at("category")},
                {"tags", note.at("tags")},
                {"created_at", note.at("created_at")},
                {"modified_at", note.at("modified_at")}
            };
            resp.set_json(response.dump());
            return;
        }
    }

    resp.status_code = 404;
    resp.set_json(pimpl_->create_error_response("Note not found", 404).dump());
}

void MobileAPI::handle_notes_add(const network::HttpRequest& req, network::HttpResponse& resp) {
    try {
        std::string user = req.headers.at("X-User");
        nlohmann::json body = nlohmann::json::parse(req.body);

        std::string id = "note_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::string title = body["title"];
        std::string content = body["content"];
        std::string category = body.value("category", "");

        auto& db = database::InternalDB::instance();
        if (db.add_note(user, id, title, content, category, {})) {
            resp.set_json(pimpl_->create_success_response("Note added successfully").dump());
        } else {
            resp.status_code = 500;
            resp.set_json(pimpl_->create_error_response("Failed to add note").dump());
        }
    } catch (...) {
        resp.status_code = 400;
        resp.set_json(pimpl_->create_error_response("Invalid request format").dump());
    }
}

// Analytics endpoints
void MobileAPI::handle_analytics_summary(const network::HttpRequest& req, network::HttpResponse& resp) {
    auto& db = database::InternalDB::instance();
    auto summary = db.get_analytics_summary();

    nlohmann::json summary_data = nlohmann::json::object();
    for (const auto& pair : summary) {
        summary_data[pair.first] = pair.second;
    }

    nlohmann::json response = pimpl_->create_success_response("Analytics summary retrieved");
    response["data"] = summary_data;
    resp.set_json(response.dump());
}

void MobileAPI::handle_analytics_insights(const network::HttpRequest& req, network::HttpResponse& resp) {
    // AI-powered insights would go here
    nlohmann::json insights = {
        {"productivity_score", 85},
        {"most_used_commands", {"git-status", "vault-list", "ai-analyze"}},
        {"peak_usage_hours", {14, 15, 16}},
        {"recommendations", {
            "Consider using AI completion more often",
            "Your vault usage is efficient",
            "Try the analytics dashboard for more insights"
        }}
    };

    nlohmann::json response = pimpl_->create_success_response("AI insights generated");
    response["data"] = insights;
    resp.set_json(response.dump());
}

// WebSocket for real-time updates
void MobileAPI::handle_websocket_connection(const std::string& message) {
    // Handle WebSocket messages for real-time updates
    LOG_INFO("WebSocket message received: " + message);
}

} // namespace mobile
} // namespace customos
