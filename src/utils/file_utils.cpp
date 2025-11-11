#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

namespace customos {
namespace utils {

// File utilities

bool file_exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool read_file(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(size);
    return file.read(reinterpret_cast<char*>(data.data()), size).good();
}

bool write_file(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

} // namespace utils
} // namespace customos
