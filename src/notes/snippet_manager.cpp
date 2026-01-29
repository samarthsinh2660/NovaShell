#include "notes/snippet_manager.h"
#include "database/internal_db.h"
#include "auth/authentication.h"
#include <map>
#include <set>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace customos {
namespace notes {

struct SnippetManager::Impl {
    std::map<std::string, Note> notes;
    std::map<std::string, CodeSnippet> snippets;
    std::string storage_path;
    std::mutex mutex;
    int next_id = 1;

    std::string generate_id() {
        return "id_" + std::to_string(next_id++);
    }
};

SnippetManager::SnippetManager() : pimpl_(std::make_unique<Impl>()) {
}

SnippetManager::~SnippetManager() = default;

SnippetManager& SnippetManager::instance() {
    static SnippetManager instance;
    return instance;
}

bool SnippetManager::initialize(const std::string& storage_path) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->storage_path = storage_path.empty() ? ".customos/notes" : storage_path;
    // TODO: Load existing notes and snippets from storage
    return true;
}

std::string SnippetManager::add_note(const std::string& title, const std::string& content, 
                                      const Tags& tags, const std::string& category) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return "";
    }

    std::string id = "note_" + std::to_string(pimpl_->next_id++);

    // Add to database
    auto& db = database::InternalDB::instance();
    if (!db.add_note(current_user, id, title, content, category, tags)) {
        return "";
    }

    // Also keep in memory for quick access
    Note note;
    note.id = id;
    note.title = title;
    note.content = content;
    note.tags = tags;
    note.category = category;
    note.created = time(nullptr);
    note.modified = note.created;
    
    pimpl_->notes[id] = note;
    return id;
}

bool SnippetManager::update_note(const std::string& id, const Note& note) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Update in database
    auto& db = database::InternalDB::instance();
    if (!db.update_note(current_user, id, note.title, note.content, note.category, note.tags)) {
        return false;
    }

    // Update in memory
    pimpl_->notes[id] = note;
    pimpl_->notes[id].modified = time(nullptr);
    return true;
}

bool SnippetManager::delete_note(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Delete from database
    auto& db = database::InternalDB::instance();
    if (!db.delete_note(current_user, id)) {
        return false;
    }

    // Delete from memory
    return pimpl_->notes.erase(id) > 0;
}

Note SnippetManager::get_note(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->notes.find(id);
    if (it != pimpl_->notes.end()) {
        return it->second;
    }
    return Note{};
}

std::vector<Note> SnippetManager::list_notes(const std::string& category) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Get from database
    auto& db = database::InternalDB::instance();
    auto notes_data = db.get_notes(current_user, category);

    std::vector<Note> result;
    for (const auto& note_data : notes_data) {
        Note note;
        note.id = note_data.at("id");
        note.title = note_data.at("title");
        note.content = note_data.at("content");
        note.category = note_data.at("category");
        
        // Parse tags JSON array
        std::string tags_json = note_data.at("tags");
        if (!tags_json.empty() && tags_json != "[]") {
            // Simple JSON parsing for tags array
            std::string clean_tags = tags_json.substr(1, tags_json.size() - 2); // Remove [ ]
            std::stringstream ss(clean_tags);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                // Remove quotes and whitespace
                tag.erase(std::remove(tag.begin(), tag.end(), '"'), tag.end());
                tag.erase(std::remove(tag.begin(), tag.end(), ' '), tag.end());
                if (!tag.empty()) {
                    note.tags.push_back(tag);
                }
            }
        }
        
        result.push_back(note);
        
        // Also update in-memory cache
        pimpl_->notes[note.id] = note;
    }
    
    return result;
}

std::vector<Note> SnippetManager::search_notes(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Search in database
    auto& db = database::InternalDB::instance();
    auto results_data = db.search_notes(current_user, query);

    std::vector<Note> result;
    for (const auto& note_data : results_data) {
        Note note;
        note.id = note_data.at("id");
        note.title = note_data.at("title");
        note.content = note_data.at("content");
        note.category = note_data.at("category");
        result.push_back(note);
    }
    
    return result;
}

std::vector<Note> SnippetManager::get_notes_by_tag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }
    
    std::vector<Note> result;
    for (const auto& pair : pimpl_->notes) {
        for (const auto& t : pair.second.tags) {
            if (t == tag) {
                result.push_back(pair.second);
                break;
            }
        }
    }
    return result;
}
}

std::string SnippetManager::add_snippet(const std::string& title, const std::string& code,
                                         const std::string& language, const std::string& description,
                                         const Tags& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return "";
    }

    std::string id = "snippet_" + std::to_string(pimpl_->next_id++);

    // Add to database
    auto& db = database::InternalDB::instance();
    if (!db.add_snippet(current_user, id, title, code, language, description, tags)) {
        return "";
    }

    // Also keep in memory for quick access
    CodeSnippet snippet;
    snippet.id = id;
    snippet.title = title;
    snippet.code = code;
    snippet.language = language;
    snippet.description = description;
    snippet.tags = tags;
    snippet.created = time(nullptr);
    snippet.modified = snippet.created;
    
    pimpl_->snippets[id] = snippet;
    return id;
}

bool SnippetManager::update_snippet(const std::string& id, const CodeSnippet& snippet) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Update in database
    auto& db = database::InternalDB::instance();
    if (!db.update_snippet(current_user, id, snippet.title, snippet.code, snippet.language, 
                          snippet.description, snippet.tags)) {
        return false;
    }

    // Update in memory
    pimpl_->snippets[id] = snippet;
    pimpl_->snippets[id].modified = time(nullptr);
    return true;
}

bool SnippetManager::delete_snippet(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return false;
    }

    // Delete from database
    auto& db = database::InternalDB::instance();
    if (!db.delete_snippet(current_user, id)) {
        return false;
    }

    // Delete from memory
    return pimpl_->snippets.erase(id) > 0;
}

CodeSnippet SnippetManager::get_snippet(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->snippets.find(id);
    if (it != pimpl_->snippets.end()) {
        return it->second;
    }
    return CodeSnippet{};
}

std::vector<CodeSnippet> SnippetManager::list_snippets(const std::string& language) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Get from database
    auto& db = database::InternalDB::instance();
    auto snippets_data = db.get_snippets(current_user, language);

    std::vector<CodeSnippet> result;
    for (const auto& snippet_data : snippets_data) {
        CodeSnippet snippet;
        snippet.id = snippet_data.at("id");
        snippet.title = snippet_data.at("title");
        snippet.code = snippet_data.at("code");
        snippet.language = snippet_data.at("language");
        snippet.description = snippet_data.at("description");
        
        // Parse tags JSON array
        std::string tags_json = snippet_data.at("tags");
        if (!tags_json.empty() && tags_json != "[]") {
            std::string clean_tags = tags_json.substr(1, tags_json.size() - 2);
            std::stringstream ss(clean_tags);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                tag.erase(std::remove(tag.begin(), tag.end(), '"'), tag.end());
                tag.erase(std::remove(tag.begin(), tag.end(), ' '), tag.end());
                if (!tag.empty()) {
                    snippet.tags.push_back(tag);
                }
            }
        }
        
        result.push_back(snippet);
        
        // Also update in-memory cache
        pimpl_->snippets[snippet.id] = snippet;
    }
    
    return result;
}

std::vector<CodeSnippet> SnippetManager::search_snippets(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Search in database
    auto& db = database::InternalDB::instance();
    auto results_data = db.search_snippets(current_user, query);

    std::vector<CodeSnippet> result;
    for (const auto& snippet_data : results_data) {
        CodeSnippet snippet;
        snippet.id = snippet_data.at("id");
        snippet.title = snippet_data.at("title");
        snippet.code = snippet_data.at("code");
        snippet.language = snippet_data.at("language");
        snippet.description = snippet_data.at("description");
        result.push_back(snippet);
    }
    
    return result;
}

std::vector<CodeSnippet> SnippetManager::get_snippets_by_tag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }
    
    std::vector<CodeSnippet> result;
    for (const auto& pair : pimpl_->snippets) {
        for (const auto& t : pair.second.tags) {
            if (t == tag) {
                result.push_back(pair.second);
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> SnippetManager::list_categories() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Get distinct categories from database
    auto& db = database::InternalDB::instance();
    auto notes_data = db.get_notes(current_user);

    std::set<std::string> categories;
    for (const auto& note_data : notes_data) {
        std::string category = note_data.at("category");
        if (!category.empty()) {
            categories.insert(category);
        }
    }
    
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> SnippetManager::list_tags() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::string current_user = auth::Authentication::instance().get_current_user();
    if (current_user.empty()) {
        return {};
    }

    // Get all tags from both notes and snippets
    auto& db = database::InternalDB::instance();
    auto notes_data = db.get_notes(current_user);
    auto snippets_data = db.get_snippets(current_user);

    std::set<std::string> tags;
    
    // Extract from notes
    for (const auto& note_data : notes_data) {
        std::string tags_json = note_data.at("tags");
        if (!tags_json.empty() && tags_json != "[]") {
            std::string clean_tags = tags_json.substr(1, tags_json.size() - 2);
            std::stringstream ss(clean_tags);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                tag.erase(std::remove(tag.begin(), tag.end(), '"'), tag.end());
                tag.erase(std::remove(tag.begin(), tag.end(), ' '), tag.end());
                if (!tag.empty()) {
                    tags.insert(tag);
                }
            }
        }
    }
    
    // Extract from snippets
    for (const auto& snippet_data : snippets_data) {
        std::string tags_json = snippet_data.at("tags");
        if (!tags_json.empty() && tags_json != "[]") {
            std::string clean_tags = tags_json.substr(1, tags_json.size() - 2);
            std::stringstream ss(clean_tags);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                tag.erase(std::remove(tag.begin(), tag.end(), '"'), tag.end());
                tag.erase(std::remove(tag.begin(), tag.end(), ' '), tag.end());
                if (!tag.empty()) {
                    tags.insert(tag);
                }
            }
        }
    }
    
    return std::vector<std::string>(tags.begin(), tags.end());
}

bool SnippetManager::export_to_markdown(const std::string& filepath) {
    // TODO: Implement markdown export
    return false;
}

bool SnippetManager::export_to_json(const std::string& filepath) {
    // TODO: Implement JSON export
    return false;
}

bool SnippetManager::import_from_file(const std::string& filepath) {
    // TODO: Implement import
    return false;
}

std::string SnippetManager::get_snippet_code(const std::string& title_or_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    // Try by ID first
    auto it = pimpl_->snippets.find(title_or_id);
    if (it != pimpl_->snippets.end()) {
        return it->second.code;
    }
    
    // Try by title
    for (const auto& pair : pimpl_->snippets) {
        if (pair.second.title == title_or_id) {
            return pair.second.code;
        }
    }
    
    return "";
}

bool SnippetManager::copy_snippet_to_clipboard(const std::string& id) {
    // TODO: Implement clipboard copy
    return false;
}

} // namespace notes
} // namespace customos
