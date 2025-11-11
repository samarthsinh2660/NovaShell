#ifndef CUSTOMOS_SNIPPET_MANAGER_H
#define CUSTOMOS_SNIPPET_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace notes {

// Note/Snippet tags
using Tags = std::vector<std::string>;

// Note entry
struct Note {
    std::string id;
    std::string title;
    std::string content;
    Tags tags;
    time_t created;
    time_t modified;
    std::string category;
};

// Code snippet
struct CodeSnippet {
    std::string id;
    std::string title;
    std::string code;
    std::string language;
    std::string description;
    Tags tags;
    time_t created;
    time_t modified;
};

// Note & Snippet Manager
class SnippetManager {
public:
    static SnippetManager& instance();

    // Initialize storage
    bool initialize(const std::string& storage_path = "");

    // Note operations
    std::string add_note(const std::string& title, const std::string& content, 
                         const Tags& tags = {}, const std::string& category = "");
    bool update_note(const std::string& id, const Note& note);
    bool delete_note(const std::string& id);
    Note get_note(const std::string& id);
    std::vector<Note> list_notes(const std::string& category = "");
    std::vector<Note> search_notes(const std::string& query);
    std::vector<Note> get_notes_by_tag(const std::string& tag);

    // Code snippet operations
    std::string add_snippet(const std::string& title, const std::string& code,
                            const std::string& language, const std::string& description = "",
                            const Tags& tags = {});
    bool update_snippet(const std::string& id, const CodeSnippet& snippet);
    bool delete_snippet(const std::string& id);
    CodeSnippet get_snippet(const std::string& id);
    std::vector<CodeSnippet> list_snippets(const std::string& language = "");
    std::vector<CodeSnippet> search_snippets(const std::string& query);
    std::vector<CodeSnippet> get_snippets_by_tag(const std::string& tag);

    // Categories and tags
    std::vector<std::string> list_categories();
    std::vector<std::string> list_tags();

    // Import/Export
    bool export_to_markdown(const std::string& filepath);
    bool export_to_json(const std::string& filepath);
    bool import_from_file(const std::string& filepath);

    // Quick access
    std::string get_snippet_code(const std::string& title_or_id);
    bool copy_snippet_to_clipboard(const std::string& id);

private:
    SnippetManager();
    ~SnippetManager();
    SnippetManager(const SnippetManager&) = delete;
    SnippetManager& operator=(const SnippetManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace notes
} // namespace customos

#endif // CUSTOMOS_SNIPPET_MANAGER_H
