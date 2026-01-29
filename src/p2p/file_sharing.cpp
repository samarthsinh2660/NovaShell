#include "p2p/file_sharing.h"
#include "database/internal_db.h"
#include "auth/authentication.h"
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace customos {
namespace p2p {

std::string generate_file_checksum(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return "";

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    SHA256_Update(&sha256, buffer, file.gcount());

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

std::string generate_share_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; ++i) {
        ss << dis(gen);
    }
    return "share_" + ss.str();
}

struct FileSharing::Impl {
    bool running = false;
    uint16_t port = 8888;
    std::vector<PeerInfo> peers;
    std::vector<SharedFile> shared_files;
    std::vector<FileTransfer> transfers;
    bool encryption_enabled = true;
    bool require_auth = false;
    TransferCallback transfer_complete_cb;
    TransferCallback transfer_progress_cb;
    std::mutex mutex;

    // P2P shared files directory
    fs::path shared_files_dir = ".customos/p2p/shares";

    // Ensure shared files directory exists
    void ensure_shared_dir() {
        try {
            fs::create_directories(shared_files_dir);
        } catch (const std::exception& e) {
            // Directory creation failed
        }
    }
};

FileSharing::FileSharing() : pimpl_(std::make_unique<Impl>()) {
}

FileSharing::~FileSharing() {
    stop_server();
}

FileSharing& FileSharing::instance() {
    static FileSharing instance;
    return instance;
}

bool FileSharing::start_server(uint16_t port) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->running) {
        return false;
    }

    pimpl_->port = port;
    // TODO: Start P2P server
    pimpl_->running = true;
    
    return true;
}

void FileSharing::stop_server() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->running = false;
}

bool FileSharing::is_running() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->running;
}

std::vector<PeerInfo> FileSharing::discover_peers() {
    // TODO: Implement peer discovery
    return {};
}

bool FileSharing::connect_to_peer(const std::string& ip_address) {
    // TODO: Connect to peer
    return false;
}

void FileSharing::disconnect_from_peer(const std::string& ip_address) {
    // TODO: Disconnect from peer
}

std::vector<PeerInfo> FileSharing::get_connected_peers() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->peers;
}

std::string FileSharing::share_file(const std::string& filepath, bool public_share) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    // Get current user
    std::string user = auth::Authentication::instance().get_current_user();
    if (user.empty()) {
        return "";
    }

    // Check if file exists
    if (!fs::exists(filepath)) {
        return "";
    }

    // Generate share ID
    std::string share_id = generate_share_id();

    // Copy file to shared directory (for local storage)
    pimpl_->ensure_shared_dir();
    fs::path source_path(filepath);
    fs::path target_path = pimpl_->shared_files_dir / share_id;
    target_path += source_path.extension();

    try {
        fs::copy_file(source_path, target_path, fs::copy_options::overwrite_existing);
    } catch (const std::exception& e) {
        return ""; // File copy failed
    }

    // Calculate file size and checksum
    std::uintmax_t file_size = fs::file_size(target_path);
    std::string checksum = generate_file_checksum(target_path.string());

    // Store metadata in database (not file content)
    auto& db = database::InternalDB::instance();
    if (!db.add_p2p_share(user, share_id, source_path.filename().string(),
                         target_path.string(), file_size, checksum, public_share)) {
        // Clean up copied file if database insert failed
        try {
            fs::remove(target_path);
        } catch (...) {}
        return "";
    }

    // Add to in-memory list
    SharedFile shared_file;
    shared_file.id = share_id;
    shared_file.filename = source_path.filename().string();
    shared_file.filepath = target_path.string();  // Local filesystem path
    shared_file.size = file_size;
    shared_file.checksum = checksum;
    shared_file.owner = user;
    shared_file.is_public = public_share;
    shared_file.public_share = public_share;  // Alias
    shared_file.shared_at = time(nullptr);

    pimpl_->shared_files.push_back(shared_file);

    return share_id;
}

bool FileSharing::unshare_file(const std::string& file_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    std::string user = auth::Authentication::instance().get_current_user();
    if (user.empty()) {
        return false;
    }

    // Remove from database
    auto& db = database::InternalDB::instance();
    if (!db.delete_p2p_share(user, file_id)) {
        return false;
    }

    // Remove physical file from local storage
    for (auto it = pimpl_->shared_files.begin(); it != pimpl_->shared_files.end(); ++it) {
        if (it->id == file_id && it->owner == user) {
            try {
                fs::remove(it->filepath);  // Delete from local filesystem
            } catch (const std::exception& e) {
                // Log error but continue
            }
            pimpl_->shared_files.erase(it);
            break;
        }
    }

    return true;
}

std::vector<SharedFile> FileSharing::list_shared_files() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    std::string user = auth::Authentication::instance().get_current_user();

    // Load from database (ensures we have latest data)
    auto& db = database::InternalDB::instance();
    auto shares_data = db.get_p2p_shares(user);

    std::vector<SharedFile> result;
    for (const auto& share : shares_data) {
        SharedFile file;
        file.id = share.at("id");
        file.filename = share.at("filename");
        file.filepath = share.at("filepath");  // Local filesystem path
        file.size = std::stoull(share.at("file_size"));
        file.checksum = share.at("checksum");
        file.owner = user;
        file.is_public = share.at("is_public") == "1";
        file.public_share = file.is_public;  // Alias
        // Parse timestamp if available
        file.shared_at = time(nullptr); // Default to now
        result.push_back(file);
    }

    return result;
}

std::vector<SharedFile> FileSharing::get_peer_files(const std::string& peer_ip) {
    // TODO: Get files from peer
    return {};
}

std::string FileSharing::request_file(const std::string& peer_ip, const std::string& file_id,
                                      const std::string& save_path) {
    // TODO: Request file from peer
    return "";
}

std::string FileSharing::send_file(const std::string& peer_ip, const std::string& filepath) {
    // TODO: Send file to peer
    return "";
}

bool FileSharing::cancel_transfer(const std::string& transfer_id) {
    // TODO: Cancel transfer
    return false;
}

std::vector<FileTransfer> FileSharing::get_active_transfers() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    return pimpl_->transfers;
}

FileTransfer FileSharing::get_transfer_status(const std::string& transfer_id) {
    // TODO: Get transfer status
    return FileTransfer{};
}

void FileSharing::set_transfer_complete_callback(TransferCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->transfer_complete_cb = callback;
}

void FileSharing::set_transfer_progress_callback(TransferCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->transfer_progress_cb = callback;
}

void FileSharing::set_encryption_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->encryption_enabled = enabled;
}

void FileSharing::set_allowed_peers(const std::vector<std::string>& peer_ips) {
    // TODO: Set allowed peers
}

void FileSharing::set_require_auth(bool require) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->require_auth = require;
}

} // namespace p2p
} // namespace customos
