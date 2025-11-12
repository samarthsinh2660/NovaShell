#include <string>
#include <vector>
#include <cstdint>
#ifdef HAVE_OPENSSL
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#endif

namespace customos {
namespace utils {

// Cryptographic utilities

std::string sha256_hash(const std::string& input) {
#ifdef HAVE_OPENSSL
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::string result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        result += buf;
    }
    return result;
#else
    // Fallback: simple hash when OpenSSL not available (NOT SECURE)
    std::string result;
    unsigned long hash_val = 5381;
    for (char c : input) {
        hash_val = ((hash_val << 5) + hash_val) + c;
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%016lx", hash_val);
    return std::string(buf);
#endif
}

std::vector<uint8_t> generate_random_bytes(size_t count) {
#ifdef HAVE_OPENSSL
    std::vector<uint8_t> bytes(count);
    RAND_bytes(bytes.data(), count);
    return bytes;
#else
    // Fallback: pseudo-random when OpenSSL not available (NOT SECURE)
    std::vector<uint8_t> bytes(count);
    for (size_t i = 0; i < count; ++i) {
        bytes[i] = static_cast<uint8_t>(rand() % 256);
    }
    return bytes;
#endif
}

// TODO: Implement AES encryption/decryption

} // namespace utils
} // namespace customos
