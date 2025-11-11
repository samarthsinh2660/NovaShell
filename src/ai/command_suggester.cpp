#include "ai/command_suggester.h"
#include <map>
#include <mutex>
#include <curl/curl.h>
#include <json/json.h>

namespace customos {
namespace ai {

struct CommandSuggester::Impl {
    std::string api_key;
    std::string api_endpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent";
    bool initialized = false;
    bool enabled = true;
    std::vector<std::string> command_history;
    std::mutex mutex;

    // Callback for HTTP response
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t total_size = size * nmemb;
        output->append((char*)contents, total_size);
        return total_size;
    }

    std::string call_gemini_api(const std::string& prompt) {
        if (!initialized || api_key.empty()) {
            return "";
        }

        CURL* curl = curl_easy_init();
        if (!curl) return "";

        std::string response;
        std::string url = api_endpoint + "?key=" + api_key;
        
        // Prepare JSON payload
        std::string json_data = R"({
            "contents": [{
                "parts": [{
                    "text": ")" + prompt + R"("
                }]
            }]
        })";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return "";
        }

        return response;
    }
};

CommandSuggester::CommandSuggester() : pimpl_(std::make_unique<Impl>()) {
}

CommandSuggester::~CommandSuggester() = default;

CommandSuggester& CommandSuggester::instance() {
    static CommandSuggester instance;
    return instance;
}

bool CommandSuggester::initialize(const std::string& api_key) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (api_key.empty()) {
        return false;
    }

    pimpl_->api_key = api_key;
    pimpl_->initialized = true;
    
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    return true;
}

bool CommandSuggester::is_initialized() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->initialized;
}

std::vector<CommandSuggestion> CommandSuggester::suggest(const SuggestionContext& context) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (!pimpl_->initialized || !pimpl_->enabled) {
        return {};
    }

    // Build prompt for Gemini
    std::string prompt = "You are a shell command assistant. Based on the following context, suggest the next most likely command:\n";
    prompt += "Current directory: " + context.current_directory + "\n";
    prompt += "User: " + context.current_user + "\n";
    prompt += "Recent commands:\n";
    for (const auto& cmd : context.recent_commands) {
        prompt += "  - " + cmd + "\n";
    }
    if (!context.partial_input.empty()) {
        prompt += "Partial input: " + context.partial_input + "\n";
    }
    prompt += "\nProvide 3-5 command suggestions with brief descriptions. Format: COMMAND|DESCRIPTION";

    std::string response = pimpl_->call_gemini_api(prompt);
    
    // Parse response and create suggestions
    // TODO: Parse JSON response from Gemini
    std::vector<CommandSuggestion> suggestions;
    
    // For now, return empty - full implementation requires JSON parsing
    return suggestions;
}

std::vector<CommandSuggestion> CommandSuggester::autocomplete(const std::string& partial) {
    // TODO: Implement autocomplete using Gemini API
    return {};
}

void CommandSuggester::learn_from_execution(const std::string& command, bool success) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->command_history.push_back(command);
    
    // Keep only last 100 commands
    if (pimpl_->command_history.size() > 100) {
        pimpl_->command_history.erase(pimpl_->command_history.begin());
    }
}

std::vector<CommandSuggestion> CommandSuggester::predict_next_command() {
    // TODO: Implement prediction based on history
    return {};
}

void CommandSuggester::enable(bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->enabled = enabled;
}

bool CommandSuggester::is_enabled() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->enabled;
}

void CommandSuggester::set_api_endpoint(const std::string& endpoint) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->api_endpoint = endpoint;
}

} // namespace ai
} // namespace customos
