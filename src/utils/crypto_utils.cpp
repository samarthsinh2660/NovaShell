#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace customos {
namespace utils {

// Cryptographic utilities

std::string sha256_hash(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::string result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        result += buf;
    }
    return result;
}

std::vector<uint8_t> generate_random_bytes(size_t count) {
    std::vector<uint8_t> bytes(count);
    RAND_bytes(bytes.data(), count);
    return bytes;
}

// TODO: Implement AES encryption/decryption

} // namespace utils
} // namespace customos
