#ifndef CUSTOMOS_FILE_SHARING_H
#define CUSTOMOS_FILE_SHARING_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace customos {
namespace p2p {

// Transfer status
enum class TransferStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    FAILED,
    CANCELLED
};

// File transfer information
struct FileTransfer {
    std::string id;
    std::string filename;
    uint64_t size;
    uint64_t transferred;
    TransferStatus status;
    std::string peer_ip;
    std::string peer_name;
    bool is_upload;
    time_t started;
    float progress_percent;
    float speed_mbps;
};

// Peer information
struct PeerInfo {
    std::string ip_address;
    std::string hostname;
    std::string username;
    bool online;
    time_t last_seen;
};

// Shared file information
struct SharedFile {
    std::string id;
    std::string filename;
    std::string path;
    uint64_t size;
    std::string description;
    bool public_share;
    std::vector<std::string> allowed_peers;
};

// Peer-to-Peer File Sharing
class FileSharing {
public:
    static FileSharing& instance();

    // Server control
    bool start_server(uint16_t port = 8888);
    void stop_server();
    bool is_running() const;

    // Peer discovery
    std::vector<PeerInfo> discover_peers();
    bool connect_to_peer(const std::string& ip_address);
    void disconnect_from_peer(const std::string& ip_address);
    std::vector<PeerInfo> get_connected_peers();

    // File sharing
    std::string share_file(const std::string& filepath, bool public_share = false);
    bool unshare_file(const std::string& file_id);
    std::vector<SharedFile> list_shared_files();
    std::vector<SharedFile> get_peer_files(const std::string& peer_ip);

    // File transfer
    std::string request_file(const std::string& peer_ip, const std::string& file_id,
                             const std::string& save_path);
    std::string send_file(const std::string& peer_ip, const std::string& filepath);
    bool cancel_transfer(const std::string& transfer_id);
    std::vector<FileTransfer> get_active_transfers();
    FileTransfer get_transfer_status(const std::string& transfer_id);

    // Callbacks
    using TransferCallback = std::function<void(const FileTransfer&)>;
    void set_transfer_complete_callback(TransferCallback callback);
    void set_transfer_progress_callback(TransferCallback callback);

    // Security
    void set_encryption_enabled(bool enabled);
    void set_allowed_peers(const std::vector<std::string>& peer_ips);
    void set_require_auth(bool require);

private:
    FileSharing();
    ~FileSharing();
    FileSharing(const FileSharing&) = delete;
    FileSharing& operator=(const FileSharing&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace p2p
} // namespace customos

#endif // CUSTOMOS_FILE_SHARING_H
