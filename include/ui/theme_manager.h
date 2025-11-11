#ifndef NOVASHELL_THEME_MANAGER_H
#define NOVASHELL_THEME_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace novashell {
namespace ui {

// Color definitions
struct Color {
    uint8_t r, g, b;
    
    Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
        : r(red), g(green), b(blue) {}
    
    std::string to_ansi() const;
    std::string to_hex() const;
};

// Theme color scheme
struct ColorScheme {
    Color background;
    Color foreground;
    Color primary;
    Color secondary;
    Color success;
    Color warning;
    Color error;
    Color info;
    Color prompt;
    Color command;
    Color output;
    Color comment;
};

// Theme configuration
struct Theme {
    std::string name;
    std::string description;
    std::string author;
    ColorScheme colors;
    std::string prompt_format;  // e.g., "{user}@{host}:{pwd}> "
    bool bold_commands;
    bool italic_comments;
    bool underline_links;
};

// Custom Themes Manager
class ThemeManager {
public:
    static ThemeManager& instance();

    // Initialize with default themes
    bool initialize();

    // Theme management
    bool load_theme(const std::string& name);
    bool save_theme(const Theme& theme);
    bool delete_theme(const std::string& name);
    Theme get_current_theme() const;
    std::vector<std::string> list_themes() const;
    
    // Built-in themes
    Theme get_default_theme();
    Theme get_dark_theme();
    Theme get_light_theme();
    Theme get_monokai_theme();
    Theme get_solarized_dark_theme();
    Theme get_solarized_light_theme();
    Theme get_dracula_theme();
    Theme get_nord_theme();
    Theme get_gruvbox_theme();

    // Custom theme creation
    bool create_custom_theme(const std::string& name, const ColorScheme& colors);
    bool edit_theme(const std::string& name, const ColorScheme& colors);
    bool duplicate_theme(const std::string& source, const std::string& new_name);

    // Color utilities
    Color parse_color(const std::string& color_str);  // #RRGGBB or rgb(r,g,b)
    std::string colorize_text(const std::string& text, const Color& color);
    std::string format_prompt(const std::string& user, const std::string& host, const std::string& pwd);

    // Import/Export
    bool import_theme_from_file(const std::string& filepath);
    bool export_theme_to_file(const std::string& theme_name, const std::string& filepath);
    bool import_from_vscode(const std::string& filepath);
    bool import_from_terminal_app(const std::string& filepath);

    // Live preview
    void preview_theme(const std::string& theme_name);
    void reset_colors();

    // Apply theme
    bool apply_theme(const std::string& name);
    bool apply_dark_mode();
    bool apply_light_mode();
    bool toggle_dark_light();

private:
    ThemeManager();
    ~ThemeManager();
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace ui
} // namespace novashell

#endif // NOVASHELL_THEME_MANAGER_H
