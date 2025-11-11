#ifndef NOVASHELL_VOICE_COMMANDER_H
#define NOVASHELL_VOICE_COMMANDER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace novashell {
namespace voice {

// Voice recognition result
struct VoiceResult {
    std::string text;
    float confidence;
    std::vector<std::string> alternatives;
};

// Voice command mapping
struct VoiceCommand {
    std::string phrase;
    std::string command;
    std::string description;
    std::vector<std::string> aliases;
    bool enabled;
};

// Voice Commands Manager
class VoiceCommander {
public:
    static VoiceCommander& instance();

    // Initialize voice recognition
    bool initialize(const std::string& api_key = "");
    bool is_initialized() const;

    // Voice recognition
    bool start_listening();
    void stop_listening();
    bool is_listening() const;
    
    // Get recognized text
    VoiceResult recognize_speech(int timeout_seconds = 5);
    VoiceResult recognize_from_audio_file(const std::string& filepath);

    // Text-to-Speech
    bool speak(const std::string& text);
    bool speak_async(const std::string& text);
    void set_voice(const std::string& voice_name);
    void set_speech_rate(float rate);  // 0.5 to 2.0
    void set_volume(float volume);     // 0.0 to 1.0

    // Command registration
    bool register_voice_command(const VoiceCommand& command);
    bool unregister_voice_command(const std::string& phrase);
    std::vector<VoiceCommand> list_voice_commands() const;
    
    // Natural language processing
    std::string parse_voice_to_command(const std::string& voice_text);
    bool execute_voice_command(const std::string& voice_text);

    // Pre-defined voice commands
    void register_default_commands();
    
    // Example commands:
    // "open file" -> vfs-ls
    // "show status" -> git status
    // "list processes" -> proc-list
    // "connect to database" -> db-connect
    // "unlock vault" -> vault-unlock

    // Wake word detection
    void set_wake_word(const std::string& word);  // e.g., "Hey Nova"
    bool enable_wake_word(bool enabled);
    
    // Callbacks
    using CommandCallback = std::function<void(const std::string& command)>;
    using ErrorCallback = std::function<void(const std::string& error)>;
    
    void set_command_callback(CommandCallback callback);
    void set_error_callback(ErrorCallback callback);

    // Audio settings
    void set_microphone_device(const std::string& device_name);
    std::vector<std::string> list_audio_devices();
    void calibrate_microphone();

    // Language settings
    void set_language(const std::string& language_code);  // "en-US", "es-ES", etc.
    std::vector<std::string> get_supported_languages();

    // Privacy
    void enable_local_processing(bool enabled);
    bool is_cloud_processing() const;
    void clear_voice_history();

private:
    VoiceCommander();
    ~VoiceCommander();
    VoiceCommander(const VoiceCommander&) = delete;
    VoiceCommander& operator=(const VoiceCommander&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace voice
} // namespace novashell

#endif // NOVASHELL_VOICE_COMMANDER_H
