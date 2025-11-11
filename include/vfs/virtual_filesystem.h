#ifndef CUSTOMOS_VIRTUAL_FILESYSTEM_H
#define CUSTOMOS_VIRTUAL_FILESYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <ctime>

namespace customos {
namespace vfs {

// File system types
enum class FSType {
    FAT32,
    NTFS,
    EXT4,
    UNKNOWN
};

// File attributes
struct FileInfo {
    std::string name;
    std::string path;
    uint64_t size;
    time_t created;
    time_t modified;
    bool is_directory;
    uint32_t permissions;
};

// Virtual file system interface
class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    virtual bool mount(const std::string& device, const std::string& mount_point) = 0;
    virtual bool unmount(const std::string& mount_point) = 0;
    virtual bool is_mounted() const = 0;

    virtual std::vector<FileInfo> list_directory(const std::string& path) = 0;
    virtual bool read_file(const std::string& path, std::vector<uint8_t>& data) = 0;
    virtual bool write_file(const std::string& path, const std::vector<uint8_t>& data) = 0;
    virtual bool delete_file(const std::string& path) = 0;
    virtual bool create_directory(const std::string& path) = 0;
    virtual bool file_exists(const std::string& path) = 0;
    virtual FileInfo get_file_info(const std::string& path) = 0;

    virtual FSType get_type() const = 0;
    virtual std::string get_mount_point() const = 0;
};

// Virtual File System manager
class VirtualFileSystem {
public:
    static VirtualFileSystem& instance();

    // Mount operations
    bool mount(const std::string& device, const std::string& mount_point, FSType type);
    bool unmount(const std::string& mount_point);
    std::vector<std::string> get_mount_points() const;

    // File operations
    std::vector<FileInfo> list(const std::string& path);
    bool copy(const std::string& source, const std::string& destination);
    bool move(const std::string& source, const std::string& destination);
    bool remove(const std::string& path);
    bool mkdir(const std::string& path);
    bool exists(const std::string& path);
    FileInfo info(const std::string& path);

    // Utility functions
    std::string resolve_path(const std::string& path);
    IFileSystem* get_filesystem(const std::string& path);

private:
    VirtualFileSystem();
    ~VirtualFileSystem();
    VirtualFileSystem(const VirtualFileSystem&) = delete;
    VirtualFileSystem& operator=(const VirtualFileSystem&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace vfs
} // namespace customos

#endif // CUSTOMOS_VIRTUAL_FILESYSTEM_H
