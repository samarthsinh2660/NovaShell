#include "ui/theme_manager.h"
#include <mutex>
#include <sstream>
#include <iomanip>

namespace novashell {
namespace ui {

std::string Color::to_ansi() const {
    std::stringstream ss;
    ss << "\033[38;2;" << (int)r << ";" << (int)g << ";" << (int)b << "m";
    return ss.str();
}

std::string Color::to_hex() const {
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') 
       << std::setw(2) << (int)r
       << std::setw(2) << (int)g
       << std::setw(2) << (int)b;
    return ss.str();
}

struct ThemeManager::Impl {
    Theme current_theme;
    std::map<std::string, Theme> themes;
    std::mutex mutex;
    bool initialized = false;
};

ThemeManager::ThemeManager() : pimpl_(std::make_unique<Impl>()) {
}

ThemeManager::~ThemeManager() = default;

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

bool ThemeManager::initialize() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    // Load built-in themes
    pimpl_->themes["default"] = get_default_theme();
    pimpl_->themes["dark"] = get_dark_theme();
    pimpl_->themes["light"] = get_light_theme();
    pimpl_->themes["monokai"] = get_monokai_theme();
    pimpl_->themes["solarized-dark"] = get_solarized_dark_theme();
    pimpl_->themes["solarized-light"] = get_solarized_light_theme();
    pimpl_->themes["dracula"] = get_dracula_theme();
    pimpl_->themes["nord"] = get_nord_theme();
    pimpl_->themes["gruvbox"] = get_gruvbox_theme();
    
    pimpl_->current_theme = pimpl_->themes["dark"];
    pimpl_->initialized = true;
    
    return true;
}

Theme ThemeManager::get_dark_theme() {
    Theme theme;
    theme.name = "Dark";
    theme.description = "Default dark theme";
    theme.colors.background = Color(30, 30, 30);
    theme.colors.foreground = Color(220, 220, 220);
    theme.colors.primary = Color(100, 150, 255);
    theme.colors.success = Color(80, 200, 120);
    theme.colors.warning = Color(255, 200, 80);
    theme.colors.error = Color(255, 100, 100);
    theme.prompt_format = "{user}@{host}:{pwd}> ";
    return theme;
}

Theme ThemeManager::get_light_theme() {
    Theme theme;
    theme.name = "Light";
    theme.description = "Light theme for daytime";
    theme.colors.background = Color(250, 250, 250);
    theme.colors.foreground = Color(30, 30, 30);
    theme.colors.primary = Color(50, 100, 200);
    theme.prompt_format = "{user}@{host}:{pwd}> ";
    return theme;
}

Theme ThemeManager::get_default_theme() {
    return get_dark_theme();
}

Theme ThemeManager::get_monokai_theme() {
    // TODO: Implement Monokai colors
    return get_dark_theme();
}

Theme ThemeManager::get_solarized_dark_theme() {
    // TODO: Implement Solarized Dark
    return get_dark_theme();
}

Theme ThemeManager::get_solarized_light_theme() {
    // TODO: Implement Solarized Light
    return get_light_theme();
}

Theme ThemeManager::get_dracula_theme() {
    // TODO: Implement Dracula colors
    return get_dark_theme();
}

Theme ThemeManager::get_nord_theme() {
    // TODO: Implement Nord colors
    return get_dark_theme();
}

Theme ThemeManager::get_gruvbox_theme() {
    // TODO: Implement Gruvbox colors
    return get_dark_theme();
}

bool ThemeManager::load_theme(const std::string& name) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->themes.find(name);
    if (it != pimpl_->themes.end()) {
        pimpl_->current_theme = it->second;
        return true;
    }
    return false;
}

Theme ThemeManager::get_current_theme() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->current_theme;
}

std::vector<std::string> ThemeManager::list_themes() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<std::string> names;
    for (const auto& pair : pimpl_->themes) {
        names.push_back(pair.first);
    }
    return names;
}

bool ThemeManager::apply_theme(const std::string& name) {
    return load_theme(name);
}

bool ThemeManager::apply_dark_mode() {
    return load_theme("dark");
}

bool ThemeManager::apply_light_mode() {
    return load_theme("light");
}

std::string ThemeManager::colorize_text(const std::string& text, const Color& color) {
    return color.to_ansi() + text + "\033[0m";
}

// Stubs for other methods
bool ThemeManager::save_theme([[maybe_unused]] const Theme& theme) { return false; }
bool ThemeManager::delete_theme([[maybe_unused]] const std::string& name) { return false; }
bool ThemeManager::create_custom_theme([[maybe_unused]] const std::string& name, [[maybe_unused]] const ColorScheme& colors) { return false; }
bool ThemeManager::edit_theme([[maybe_unused]] const std::string& name, [[maybe_unused]] const ColorScheme& colors) { return false; }
bool ThemeManager::duplicate_theme([[maybe_unused]] const std::string& source, [[maybe_unused]] const std::string& new_name) { return false; }
Color ThemeManager::parse_color([[maybe_unused]] const std::string& color_str) { return Color(); }
std::string ThemeManager::format_prompt([[maybe_unused]] const std::string& user, [[maybe_unused]] const std::string& host, [[maybe_unused]] const std::string& pwd) { return ""; }
bool ThemeManager::import_theme_from_file([[maybe_unused]] const std::string& filepath) { return false; }
bool ThemeManager::export_theme_to_file([[maybe_unused]] const std::string& theme_name, [[maybe_unused]] const std::string& filepath) { return false; }
bool ThemeManager::import_from_vscode([[maybe_unused]] const std::string& filepath) { return false; }
bool ThemeManager::import_from_terminal_app([[maybe_unused]] const std::string& filepath) { return false; }
void ThemeManager::preview_theme([[maybe_unused]] const std::string& theme_name) {}
void ThemeManager::reset_colors() {}
bool ThemeManager::toggle_dark_light() { return false; }

} // namespace ui
} // namespace novashell
