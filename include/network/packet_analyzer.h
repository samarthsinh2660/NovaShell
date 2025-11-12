#ifndef CUSTOMOS_PACKET_ANALYZER_H
#define CUSTOMOS_PACKET_ANALYZER_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <functional>

namespace customos {
namespace network {

// Protocol types
enum class Protocol {
    TCP,
    UDP,
    ICMP,
    ARP,
    DNS,
    HTTP,
    HTTPS,
    FTP,
    SSH,
    UNKNOWN
};

// Packet information
struct PacketInfo {
    Protocol protocol;
    std::string src_ip;
    std::string dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    size_t size;
    time_t timestamp;
    std::vector<uint8_t> payload;
    std::string protocol_info;
};

// Network statistics
struct NetworkStats {
    uint64_t total_packets;
    uint64_t total_bytes;
    uint64_t tcp_packets;
    uint64_t udp_packets;
    uint64_t icmp_packets;
    uint64_t other_packets;
    std::map<std::string, uint64_t> top_talkers;
};

// Packet filter
struct PacketFilter {
    std::string protocol;
    std::string src_ip;
    std::string dst_ip;
    uint16_t port;
    size_t min_size;
    size_t max_size;
};

// Packet callback
using PacketCallback = std::function<void(const PacketInfo&)>;

// Packet Analyzer
class PacketAnalyzer {
public:
    static PacketAnalyzer& instance();

    // Capture control
    bool start_capture(const std::string& interface, const PacketFilter& filter = PacketFilter());
    void stop_capture();
    bool is_capturing() const;

    // Callbacks
    void set_packet_callback(PacketCallback callback);

    // Statistics
    NetworkStats get_statistics() const;
    void reset_statistics();

    // Packet storage
    bool save_capture(const std::string& filepath);
    bool load_capture(const std::string& filepath);

    // Interface management
    std::vector<std::string> list_interfaces() const;
    std::string get_default_interface() const;

    // Packet analysis
    std::vector<PacketInfo> get_packets(size_t max_count = 100) const;
    std::vector<PacketInfo> filter_packets(const PacketFilter& filter) const;

    // Protocol analysis
    std::string analyze_protocol(const PacketInfo& packet) const;
    std::map<Protocol, uint64_t> get_protocol_distribution() const;

private:
    PacketAnalyzer();
    ~PacketAnalyzer();
    PacketAnalyzer(const PacketAnalyzer&) = delete;
    PacketAnalyzer& operator=(const PacketAnalyzer&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace network
} // namespace customos

#endif // CUSTOMOS_PACKET_ANALYZER_H
