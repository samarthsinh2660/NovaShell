#include "remote/remote_desktop.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <wingdi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define close closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace customos {
namespace remote {

struct RemoteDesktopServer::Impl {
    std::atomic<bool> running{false};
    int port = 5900;
    int server_socket = -1;
    std::vector<std::thread> client_threads;
    std::mutex sessions_mutex;
    std::unordered_map<std::string, RemoteSession> active_sessions;
    std::mutex callbacks_mutex;
    std::function<bool(const std::string&)> auth_callback;
    std::function<void(const RemoteSession&)> session_callback;

    // Configuration
    int capture_interval_ms = 100;
    int quality = 80;
    bool compression_enabled = true;
    RemoteDesktopConfig config;
    CaptureMode capture_mode = CaptureMode::FULL_DESKTOP;
    std::string active_display;

    // Screen capture
    ScreenCapture last_capture;

    void accept_clients() {
        while (running) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
            if (client_socket >= 0) {
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

                client_threads.emplace_back(&Impl::client_handler_thread, this, std::string(client_ip), client_socket);
            }
        }
    }

    void client_handler_thread(const std::string& client_ip, int client_socket) {
        // Authenticate client
        if (!authenticate_client(client_socket)) {
            close(client_socket);
            return;
        }

        // Create session
        RemoteSession session;
        session.session_id = generate_session_id();
        session.client_ip = client_ip;
        session.authenticated = true;
        session.connected_at = std::chrono::system_clock::now();
        session.last_activity = session.connected_at;

        {
            std::lock_guard<std::mutex> lock(sessions_mutex);
            active_sessions[session.session_id] = session;
        }

        // Notify session callback
        {
            std::lock_guard<std::mutex> lock(callbacks_mutex);
            if (session_callback) {
                session_callback(session);
            }
        }

        // Handle client commands
        handle_client_commands(client_socket, session);

        // Remove session
        {
            std::lock_guard<std::mutex> lock(sessions_mutex);
            active_sessions.erase(session.session_id);
        }

        close(client_socket);
    }

    bool authenticate_client(int client_socket) {
        // Simple authentication - send challenge
        const char* challenge = "RFB 003.008\n";
        send(client_socket, challenge, strlen(challenge), 0);

        // For now, accept all connections (in production, implement proper auth)
        std::lock_guard<std::mutex> lock(callbacks_mutex);
        if (auth_callback) {
            // Would implement password exchange here
            return auth_callback(""); // Accept all for now
        }

        return true;
    }

    void handle_client_commands(int client_socket, const RemoteSession& session) {
        // Simple command handling loop
        while (running) {
            char buffer[1024];
            int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytes_read <= 0) break;

            // Parse and handle commands
            // This is a simplified implementation
            if (buffer[0] == 0) { // Screen update request
                auto capture = capture_screen();
                send_screen_update(session.session_id, capture);
            }
        }
    }

    std::string generate_session_id() {
        static int counter = 0;
        return "session_" + std::to_string(++counter) + "_" + std::to_string(time(nullptr));
    }
};

RemoteDesktopServer::RemoteDesktopServer() : pimpl_(std::make_unique<Impl>()) {
}

RemoteDesktopServer& RemoteDesktopServer::instance() {
    static RemoteDesktopServer instance;
    return instance;
}

RemoteDesktopServer::~RemoteDesktopServer() {
    stop();
}

bool RemoteDesktopServer::start(int port) {
    pimpl_->port = port;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }
#endif

    pimpl_->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (pimpl_->server_socket < 0) {
        return false;
    }

    int opt = 1;
    setsockopt(pimpl_->server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(pimpl_->server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return false;
    }

    if (listen(pimpl_->server_socket, 10) < 0) {
        return false;
    }

    pimpl_->running = true;
    std::thread(&Impl::accept_clients, pimpl_.get()).detach();

    return true;
}

void RemoteDesktopServer::stop() {
    if (!pimpl_->running) return;

    pimpl_->running = false;

#ifdef _WIN32
    closesocket(pimpl_->server_socket);
#else
    close(pimpl_->server_socket);
#endif

    for (auto& thread : pimpl_->client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    pimpl_->client_threads.clear();
    pimpl_->active_sessions.clear();
}

bool RemoteDesktopServer::is_running() const {
    return pimpl_->running;
}

std::vector<RemoteSession> RemoteDesktopServer::get_active_sessions() const {
    std::lock_guard<std::mutex> lock(pimpl_->sessions_mutex);
    std::vector<RemoteSession> sessions;
    for (const auto& pair : pimpl_->active_sessions) {
        sessions.push_back(pair.second);
    }
    return sessions;
}

bool RemoteDesktopServer::disconnect_session(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(pimpl_->sessions_mutex);
    return pimpl_->active_sessions.erase(session_id) > 0;
}

ScreenCapture RemoteDesktopServer::capture_screen() {
#ifdef _WIN32
    return capture_screen_windows();
#else
    return capture_screen_linux();
#endif
}

bool RemoteDesktopServer::send_screen_update(const std::string& session_id, const ScreenCapture& capture) {
    // Implementation would send screen data to client
    // For now, just update last capture
    pimpl_->last_capture = capture;
    return true;
}

void RemoteDesktopServer::handle_mouse_event(const MouseEvent& event) {
    // Handle mouse input from remote client
#ifdef _WIN32
    // Windows mouse event simulation
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dx = event.x;
    input.mi.dy = event.y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    if (event.button == 0) {
        input.mi.dwFlags |= event.pressed ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    }

    SendInput(1, &input, sizeof(INPUT));
#endif
}

void RemoteDesktopServer::handle_keyboard_event(const KeyboardEvent& event) {
    // Handle keyboard input from remote client
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = event.key_code;
    input.ki.dwFlags = event.pressed ? 0 : KEYEVENTF_KEYUP;

    if (event.ctrl) input.ki.dwFlags |= KEYEVENTF_CONTROL;
    if (event.alt) input.ki.dwFlags |= KEYEVENTF_ALT;
    if (event.shift) input.ki.dwFlags |= KEYEVENTF_SHIFT;

    SendInput(1, &input, sizeof(INPUT));
#endif
}

void RemoteDesktopServer::set_capture_interval(int milliseconds) {
    pimpl_->capture_interval_ms = milliseconds;
}

void RemoteDesktopServer::set_quality(int quality) {
    pimpl_->quality = std::max(1, std::min(100, quality));
}

void RemoteDesktopServer::enable_compression(bool enable) {
    pimpl_->compression_enabled = enable;
}

void RemoteDesktopServer::set_auth_callback(std::function<bool(const std::string& password)> callback) {
    std::lock_guard<std::mutex> lock(pimpl_->callbacks_mutex);
    pimpl_->auth_callback = callback;
}

void RemoteDesktopServer::set_session_callback(std::function<void(const RemoteSession&)> session_callback) {
    std::lock_guard<std::mutex> lock(pimpl_->callbacks_mutex);
    pimpl_->session_callback = session_callback;
}

// Screen capture implementations
ScreenCapture capture_screen_windows() {
    ScreenCapture capture;
    capture.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);

    BitBlt(hDC, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Negative for top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    int data_size = width * height * 4;
    capture.data.resize(data_size);
    GetDIBits(hDC, hBitmap, 0, height, capture.data.data(), &bmi, DIB_RGB_COLORS);

    capture.width = width;
    capture.height = height;
    capture.bpp = 32;

    // Cleanup
    SelectObject(hDC, old_obj);
    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);

    return capture;
}

ScreenCapture capture_screen_linux() {
    ScreenCapture capture;
    capture.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    // X11 screen capture implementation
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return capture;  // Return empty capture on error
    }

    Window root = DefaultRootWindow(display);
    XWindowAttributes attrs;
    XGetWindowAttributes(display, root, &attrs);

    int width = attrs.width;
    int height = attrs.height;

    XImage* image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
    if (image) {
        capture.width = width;
        capture.height = height;
        capture.bpp = image->bits_per_pixel;

        size_t data_size = image->bytes_per_line * height;
        capture.data.resize(data_size);
        memcpy(capture.data.data(), image->data, data_size);

        XDestroyImage(image);
    }

    XCloseDisplay(display);
    return capture;
}

void RemoteDesktopServer::set_capture_mode(CaptureMode mode) {
    pimpl_->capture_mode = mode;
}

void RemoteDesktopServer::set_capture_config(const RemoteDesktopConfig& config) {
    pimpl_->config = config;
    pimpl_->capture_mode = config.capture_mode;
}

RemoteDesktopConfig RemoteDesktopServer::get_capture_config() const {
    return pimpl_->config;
}

std::vector<TerminalSession> RemoteDesktopServer::enumerate_terminal_sessions() {
    std::vector<TerminalSession> sessions;

#ifdef _WIN32
    // Enumerate terminal windows on Windows
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        auto sessions = reinterpret_cast<std::vector<TerminalSession>*>(lParam);

        char title[256];
        GetWindowTextA(hwnd, title, sizeof(title));

        // Check if it's a terminal window
        if (strstr(title, "Command Prompt") ||
            strstr(title, "PowerShell") ||
            strstr(title, "Terminal") ||
            strstr(title, "Console") ||
            strstr(title, "cmd.exe") ||
            strstr(title, "powershell.exe")) {

            DWORD processId;
            GetWindowThreadProcessId(hwnd, &processId);

            RECT bounds;
            GetWindowRect(hwnd, &bounds);

            TerminalSession session;
            session.session_id = "term_" + std::to_string((uintptr_t)hwnd);
            session.window_title = title;
            session.process_id = processId;
            session.is_active = (GetForegroundWindow() == hwnd);
            session.window_bounds = bounds;

            // Determine terminal type
            if (strstr(title, "PowerShell")) {
                session.terminal_type = "powershell";
            } else if (strstr(title, "Command Prompt") || strstr(title, "cmd.exe")) {
                session.terminal_type = "cmd";
            } else {
                session.terminal_type = "terminal";
            }

            sessions->push_back(session);
        }

        return TRUE;
    }, reinterpret_cast<LPARAM>(&sessions));

#else
    // Linux terminal enumeration (simplified)
    // This would use X11 to enumerate windows and check for terminal processes
    // For now, return empty list
#endif

    return sessions;
}

bool RemoteDesktopServer::switch_to_terminal_session(const std::string& session_id) {
#ifdef _WIN32
    // Extract window handle from session ID
    size_t prefix_len = strlen("term_");
    if (session_id.substr(0, prefix_len) != "term_") {
        return false;
    }

    uintptr_t hwnd_val = std::stoull(session_id.substr(prefix_len));
    HWND hwnd = reinterpret_cast<HWND>(hwnd_val);

    if (IsWindow(hwnd)) {
        return bring_window_to_front(hwnd);
    }
#endif
    return false;
}

bool RemoteDesktopServer::capture_terminal_session(const std::string& session_id, ScreenCapture& capture) {
#ifdef _WIN32
    // Extract window handle from session ID
    size_t prefix_len = strlen("term_");
    if (session_id.substr(0, prefix_len) != "term_") {
        return false;
    }

    uintptr_t hwnd_val = std::stoull(session_id.substr(prefix_len));
    HWND hwnd = reinterpret_cast<HWND>(hwnd_val);

    return capture_window(hwnd, capture);
#endif
    return false;
}

bool RemoteDesktopServer::send_terminal_input(const std::string& session_id, const std::string& input) {
#ifdef _WIN32
    // Extract window handle from session ID
    size_t prefix_len = strlen("term_");
    if (session_id.substr(0, prefix_len) != "term_") {
        return false;
    }

    uintptr_t hwnd_val = std::stoull(session_id.substr(prefix_len));
    HWND hwnd = reinterpret_cast<HWND>(hwnd_val);

    if (!IsWindow(hwnd)) {
        return false;
    }

    // Bring window to front first
    bring_window_to_front(hwnd);

    // Send input as keyboard events
    for (char c : input) {
        // Simulate key press and release
        INPUT inputs[2] = {0};

        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = 0; // Virtual key code (we'll use scan code)
        inputs[0].ki.wScan = c;
        inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

        inputs[1] = inputs[0];
        inputs[1].ki.dwFlags |= KEYEVENTF_KEYUP;

        SendInput(2, inputs, sizeof(INPUT));
    }

    return true;
#endif
    return false;
}

std::vector<std::string> RemoteDesktopServer::enumerate_displays() {
    std::vector<std::string> displays;

#ifdef _WIN32
    DISPLAY_DEVICE dd = {sizeof(dd)};
    int deviceIndex = 0;

    while (EnumDisplayDevices(NULL, deviceIndex, &dd, 0)) {
        displays.push_back(std::string(dd.DeviceName));
        deviceIndex++;
    }
#else
    // Linux display enumeration
    displays.push_back("default"); // Simplified
#endif

    return displays;
}

bool RemoteDesktopServer::set_active_display(const std::string& display_name) {
    auto displays = enumerate_displays();
    if (std::find(displays.begin(), displays.end(), display_name) != displays.end()) {
        pimpl_->active_display = display_name;
        return true;
    }
    return false;
}

ScreenCapture RemoteDesktopServer::capture_full_desktop() {
    pimpl_->capture_mode = CaptureMode::FULL_DESKTOP;
    return capture_screen();
}

ScreenCapture RemoteDesktopServer::capture_display(const std::string& display_name) {
    pimpl_->active_display = display_name;
    return capture_screen();
}

std::vector<std::pair<HWND, std::string>> RemoteDesktopServer::enumerate_windows() {
    std::vector<std::pair<HWND, std::string>> windows;

#ifdef _WIN32
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        auto windows = reinterpret_cast<std::vector<std::pair<HWND, std::string>>*>(lParam);

        if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0) {
            char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
            windows->emplace_back(hwnd, std::string(title));
        }

        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
#endif

    return windows;
}

bool RemoteDesktopServer::capture_window(HWND hwnd, ScreenCapture& capture) {
#ifdef _WIN32
    if (!IsWindow(hwnd)) {
        return false;
    }

    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    if (width <= 0 || height <= 0) {
        return false;
    }

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);

    if (!hBitmap) {
        DeleteDC(hDC);
        ReleaseDC(NULL, hScreen);
        return false;
    }

    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);

    // Capture the window
    if (!PrintWindow(hwnd, hDC, PW_CLIENTONLY)) {
        // Fallback to BitBlt if PrintWindow fails
        BitBlt(hDC, 0, 0, width, height, hScreen, windowRect.left, windowRect.top, SRCCOPY);
    }

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Negative for top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    int data_size = width * height * 4;
    capture.data.resize(data_size);

    if (GetDIBits(hDC, hBitmap, 0, height, capture.data.data(), &bmi, DIB_RGB_COLORS)) {
        capture.width = width;
        capture.height = height;
        capture.bpp = 32;
        capture.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        capture.is_fullscreen = false;
    }

    // Cleanup
    SelectObject(hDC, old_obj);
    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);

    return !capture.data.empty();
#endif
    return false;
}

bool RemoteDesktopServer::bring_window_to_front(HWND hwnd) {
#ifdef _WIN32
    if (!IsWindow(hwnd)) {
        return false;
    }

    // Bring window to front
    if (SetForegroundWindow(hwnd)) {
        // Also make it the topmost window temporarily, then restore
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        return true;
    }
#endif
    return false;
}

RemoteDesktopClient::~RemoteDesktopClient() {
    disconnect();
}

bool RemoteDesktopClient::connect(const std::string& host, int port, const std::string& password) {
    // Implementation would connect to remote desktop server
    return false;
}

void RemoteDesktopClient::disconnect() {
    // Implementation would disconnect from server
}

bool RemoteDesktopClient::is_connected() const {
    return false;
}

ScreenCapture RemoteDesktopClient::receive_screen_update() {
    return ScreenCapture{};
}

void RemoteDesktopClient::request_full_update() {
    // Implementation would request full screen update
}

void RemoteDesktopClient::send_mouse_event(const MouseEvent& event) {
    // Implementation would send mouse event to server
}

void RemoteDesktopClient::send_keyboard_event(const KeyboardEvent& event) {
    // Implementation would send keyboard event to server
}

void RemoteDesktopClient::set_update_callback(std::function<void(const ScreenCapture&)> callback) {
    // Implementation would set screen update callback
}

// Utility functions
std::string compress_screen_data(const std::vector<uint8_t>& data) {
    // Simple RLE compression for now
    std::string compressed;
    if (data.empty()) return compressed;

    uint8_t current = data[0];
    int count = 1;

    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current && count < 255) {
            count++;
        } else {
            compressed += static_cast<char>(count);
            compressed += static_cast<char>(current);
            current = data[i];
            count = 1;
        }
    }

    compressed += static_cast<char>(count);
    compressed += static_cast<char>(current);

    return compressed;
}

std::vector<uint8_t> decompress_screen_data(const std::string& compressed_data) {
    std::vector<uint8_t> decompressed;

    for (size_t i = 0; i < compressed_data.size(); i += 2) {
        if (i + 1 >= compressed_data.size()) break;

        int count = static_cast<uint8_t>(compressed_data[i]);
        uint8_t value = static_cast<uint8_t>(compressed_data[i + 1]);

        for (int j = 0; j < count; ++j) {
            decompressed.push_back(value);
        }
    }

    return decompressed;
}

} // namespace remote
} // namespace customos
