#ifndef CUSTOMOS_REMOTE_DESKTOP_H
#define CUSTOMOS_REMOTE_DESKTOP_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
// Cross-platform rectangle structure
struct RECT {
    long left;
    long top;
    long right;
    long bottom;
};
#endif

namespace customos {
namespace remote {

struct ScreenCapture {
    std::vector<uint8_t> data;
    int width;
    int height;
    int bpp;  // bits per pixel
    uint64_t timestamp;
    std::string display_name;  // Which display/screen
    bool is_fullscreen = true; // Full screen or specific window
};

struct TerminalSession {
    std::string session_id;
    std::string terminal_type;  // "bash", "zsh", "powershell", etc.
    std::string working_directory;
    bool is_active;
    std::string window_title;
    RECT window_bounds;  // Windows coordinates
    uint32_t process_id; // Terminal process ID
};

enum class CaptureMode {
    FULL_DESKTOP,      // Entire desktop across all monitors
    PRIMARY_MONITOR,   // Only primary monitor
    ACTIVE_WINDOW,     // Currently focused window
    SPECIFIC_WINDOW,   // Specific window by handle/ID
    TERMINAL_SESSION,  // Specific terminal window
    CUSTOM_REGION      // User-defined rectangle
};

struct RemoteDesktopConfig {
    CaptureMode capture_mode = CaptureMode::FULL_DESKTOP;
    int capture_fps = 30;
    int quality = 80;  // 1-100
    bool enable_compression = true;
    bool capture_cursor = true;
    bool capture_audio = false;
    std::string preferred_display;  // For multi-monitor setups
    std::vector<TerminalSession> terminal_sessions;
};

struct MouseEvent {
    int x, y;
    int button;  // 0=left, 1=middle, 2=right
    bool pressed;
};

struct KeyboardEvent {
    int key_code;
    bool pressed;
    bool ctrl, alt, shift;
};

struct RemoteSession {
    std::string session_id;
    std::string client_ip;
    bool authenticated;
    std::chrono::system_clock::time_point connected_at;
    std::chrono::system_clock::time_point last_activity;
};

class RemoteDesktopServer {
public:
    static RemoteDesktopServer& instance();

    RemoteDesktopServer();
    ~RemoteDesktopServer();

    // Server management
    bool start(int port = 5900);
    void stop();
    bool is_running() const;

    // Session management
    std::vector<RemoteSession> get_active_sessions() const;
    bool disconnect_session(const std::string& session_id);

    // Screen capture
    ScreenCapture capture_screen();
    bool send_screen_update(const std::string& session_id, const ScreenCapture& capture);

    // Input handling
    void handle_mouse_event(const MouseEvent& event);
    void handle_keyboard_event(const KeyboardEvent& event);

    // Configuration
    void set_capture_interval(int milliseconds);
    void set_quality(int quality);  // 1-100
    void enable_compression(bool enable);
    void set_capture_mode(CaptureMode mode);
    void set_capture_config(const RemoteDesktopConfig& config);
    RemoteDesktopConfig get_capture_config() const;

    // Terminal session management
    std::vector<TerminalSession> enumerate_terminal_sessions();
    bool switch_to_terminal_session(const std::string& session_id);
    bool capture_terminal_session(const std::string& session_id, ScreenCapture& capture);
    bool send_terminal_input(const std::string& session_id, const std::string& input);

    // Full screen and multi-monitor support
    std::vector<std::string> enumerate_displays();
    bool set_active_display(const std::string& display_name);
    ScreenCapture capture_full_desktop();
    ScreenCapture capture_display(const std::string& display_name);

    // Window management
    std::vector<std::pair<HWND, std::string>> enumerate_windows(); // Windows-specific
    bool capture_window(HWND hwnd, ScreenCapture& capture);
    bool bring_window_to_front(HWND hwnd);

    // Callbacks
    void set_auth_callback(std::function<bool(const std::string& password)> callback);
    void set_session_callback(std::function<void(const RemoteSession&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

    void client_handler_thread(const std::string& client_ip, int client_socket);
    bool authenticate_client(int client_socket);
    void handle_client_commands(int client_socket, const RemoteSession& session);
};

class RemoteDesktopClient {
public:
    RemoteDesktopClient();
    ~RemoteDesktopClient();

    // Connection management
    bool connect(const std::string& host, int port, const std::string& password = "");
    void disconnect();
    bool is_connected() const;

    // Screen viewing
    ScreenCapture receive_screen_update();
    void request_full_update();

    // Input sending
    void send_mouse_event(const MouseEvent& event);
    void send_keyboard_event(const KeyboardEvent& event);

    // Configuration
    void set_update_callback(std::function<void(const ScreenCapture&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Utility functions
std::string compress_screen_data(const std::vector<uint8_t>& data);
std::vector<uint8_t> decompress_screen_data(const std::string& compressed_data);
ScreenCapture capture_screen_windows();
ScreenCapture capture_screen_linux();

} // namespace remote
} // namespace customos

#endif // CUSTOMOS_REMOTE_DESKTOP_H
