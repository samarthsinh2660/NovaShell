#include "vfs/virtual_filesystem.h"
#include <map>
#include <mutex>
#include <memory>

namespace customos {
namespace vfs {

// Pimpl implementation
struct VirtualFileSystem::Impl {
    std::map<std::string, std::unique_ptr<IFileSystem>> mounted_filesystems;
    std::mutex mutex;
};

VirtualFileSystem::VirtualFileSystem() : pimpl_(std::make_unique<Impl>()) {
}

VirtualFileSystem::~VirtualFileSystem() = default;

VirtualFileSystem& VirtualFileSystem::instance() {
    static VirtualFileSystem instance;
    return instance;
}

bool VirtualFileSystem::mount(const std::string& device, const std::string& mount_point, FSType type) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    // TODO: Create appropriate filesystem implementation based on type
    // For now, just log the mount operation
    (void)device;    // Suppress unused parameter warning
    (void)mount_point; // Suppress unused parameter warning
    (void)type;      // Suppress unused parameter warning
    return true;
}

bool VirtualFileSystem::unmount(const std::string& mount_point) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->mounted_filesystems.find(mount_point);
    if (it != pimpl_->mounted_filesystems.end()) {
        it->second->unmount(mount_point);
        pimpl_->mounted_filesystems.erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::string> VirtualFileSystem::get_mount_points() const {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<std::string> points;
    for (const auto& pair : pimpl_->mounted_filesystems) {
        points.push_back(pair.first);
    }
    return points;
}

std::vector<FileInfo> VirtualFileSystem::list(const std::string& path) {
    IFileSystem* fs = get_filesystem(path);
    if (fs) {
        return fs->list_directory(path);
    }
    return {};
}

bool VirtualFileSystem::copy(const std::string& source, const std::string& destination) {
    // TODO: Implement file copy
    (void)source;      // Suppress unused parameter warning
    (void)destination; // Suppress unused parameter warning
    return false;
}

bool VirtualFileSystem::move(const std::string& source, const std::string& destination) {
    // TODO: Implement file move
    (void)source;      // Suppress unused parameter warning
    (void)destination; // Suppress unused parameter warning
    return false;
}

bool VirtualFileSystem::remove(const std::string& path) {
    IFileSystem* fs = get_filesystem(path);
    if (fs) {
        return fs->delete_file(path);
    }
    return false;
}

bool VirtualFileSystem::mkdir(const std::string& path) {
    IFileSystem* fs = get_filesystem(path);
    if (fs) {
        return fs->create_directory(path);
    }
    return false;
}

bool VirtualFileSystem::exists(const std::string& path) {
    IFileSystem* fs = get_filesystem(path);
    if (fs) {
        return fs->file_exists(path);
    }
    return false;
}

FileInfo VirtualFileSystem::info(const std::string& path) {
    IFileSystem* fs = get_filesystem(path);
    if (fs) {
        return fs->get_file_info(path);
    }
    return FileInfo{};
}

std::string VirtualFileSystem::resolve_path(const std::string& path) {
    // TODO: Implement path resolution
    return path;
}

IFileSystem* VirtualFileSystem::get_filesystem(const std::string& path) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    // Find the mounted filesystem for this path
    for (auto& pair : pimpl_->mounted_filesystems) {
        if (path.find(pair.first) == 0) {
            return pair.second.get();
        }
    }
    
    return nullptr;
}

} // namespace vfs
} // namespace customos
