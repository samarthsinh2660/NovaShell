#include "notes/snippet_manager.h"
#include <map>
#include <set>
#include <mutex>
#include <fstream>
#include <sstream>

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
    
    Note note;
    note.id = pimpl_->generate_id();
    note.title = title;
    note.content = content;
    note.tags = tags;
    note.category = category;
    note.created = time(nullptr);
    note.modified = note.created;
    
    pimpl_->notes[note.id] = note;
    return note.id;
}

bool SnippetManager::update_note(const std::string& id, const Note& note) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->notes.find(id);
    if (it != pimpl_->notes.end()) {
        it->second = note;
        it->second.modified = time(nullptr);
        return true;
    }
    return false;
}

bool SnippetManager::delete_note(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
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
    
    std::vector<Note> result;
    for (const auto& pair : pimpl_->notes) {
        if (category.empty() || pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Note> SnippetManager::search_notes(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<Note> result;
    for (const auto& pair : pimpl_->notes) {
        if (pair.second.title.find(query) != std::string::npos ||
            pair.second.content.find(query) != std::string::npos) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Note> SnippetManager::get_notes_by_tag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
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

std::string SnippetManager::add_snippet(const std::string& title, const std::string& code,
                                         const std::string& language, const std::string& description,
                                         const Tags& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    CodeSnippet snippet;
    snippet.id = pimpl_->generate_id();
    snippet.title = title;
    snippet.code = code;
    snippet.language = language;
    snippet.description = description;
    snippet.tags = tags;
    snippet.created = time(nullptr);
    snippet.modified = snippet.created;
    
    pimpl_->snippets[snippet.id] = snippet;
    return snippet.id;
}

bool SnippetManager::update_snippet(const std::string& id, const CodeSnippet& snippet) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->snippets.find(id);
    if (it != pimpl_->snippets.end()) {
        it->second = snippet;
        it->second.modified = time(nullptr);
        return true;
    }
    return false;
}

bool SnippetManager::delete_snippet(const std::string& id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
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
    
    std::vector<CodeSnippet> result;
    for (const auto& pair : pimpl_->snippets) {
        if (language.empty() || pair.second.language == language) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<CodeSnippet> SnippetManager::search_snippets(const std::string& query) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<CodeSnippet> result;
    for (const auto& pair : pimpl_->snippets) {
        if (pair.second.title.find(query) != std::string::npos ||
            pair.second.code.find(query) != std::string::npos ||
            pair.second.description.find(query) != std::string::npos) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<CodeSnippet> SnippetManager::get_snippets_by_tag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
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
    
    std::set<std::string> categories;
    for (const auto& pair : pimpl_->notes) {
        if (!pair.second.category.empty()) {
            categories.insert(pair.second.category);
        }
    }
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> SnippetManager::list_tags() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::set<std::string> tags;
    for (const auto& pair : pimpl_->notes) {
        for (const auto& tag : pair.second.tags) {
            tags.insert(tag);
        }
    }
    for (const auto& pair : pimpl_->snippets) {
        for (const auto& tag : pair.second.tags) {
            tags.insert(tag);
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
