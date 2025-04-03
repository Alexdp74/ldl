#include <cstdint>
#include <array>


// Ethernet Frame Header
struct mac
{
    static constexpr size_t MAC_ADDRESS_LENGTH{6U};

    template<typename... B> requires(sizeof...(B) == MAC_ADDRESS_LENGTH) mac (B... bytes) noexcept : address{static_cast<uint8_t>(bytes)...} { }

    [[nodiscard]] bool friend operator== (const mac & lhs, const mac & rhs) noexcept
    {
        return[lhs, rhs]<size_t... Is> (std::index_sequence<Is...>) {
            return ((lhs.address[Is] == rhs.address[Is]) && ...);
        } (std::make_index_sequence<MAC_ADDRESS_LENGTH>());
    }


    uint8_t address[MAC_ADDRESS_LENGTH];   // MAC Address
};

struct eth_header_composed
{
    mac dest_mac;           // Destination MAC Address
    mac src_mac;            // Source MAC Address
    uint16_t ethertype;     // EtherType (e.g., 0x0800 for IPv4)
};

struct eth_header
{
    std::array<uint8_t, 6U> dest_mac;   // Destination MAC Address
    std::array<uint8_t, 6U> src_mac;    // Source MAC Address
    uint16_t ethertype;                 // EtherType (e.g., 0x0800 for IPv4)
};

// IPv4 Header (20 bytes minimum)
struct ip_header
{
    uint8_t  ihl_version;       // Internet Header Length (4-bit) + Version (4-bit, usually 4 for IPv4)
    uint8_t  dscp_ecn;          // Differentiated Services and ECN
    uint16_t total_length;      // Total length of the packet (header + data)
    uint16_t identification;    // Identification field
    uint16_t flags_frag_offset; // Flags (3 bits) + Fragment Offset (13 bits)
    uint8_t  ttl;               // Time To Live
    uint8_t  protocol;          // Protocol (e.g., 6 for TCP)
    uint16_t checksum;          // Header checksum
    uint32_t src_ip;            // Source IP Address
    uint32_t dest_ip;           // Destination IP Address
};

// TCP Header (20 bytes minimum)
struct tcp_header
{
    uint16_t src_port;          // Source Port
    uint16_t dest_port;         // Destination Port
    uint32_t seq_number;        // Sequence Number
    uint32_t ack_number;        // Acknowledgment Number
    uint8_t  data_offset_rsvd;  // Data Offset (Header Length in 32-bit words) + reserved flags
    uint8_t  flags;             // TCP Flags (e.g., SYN, ACK, FIN, etc.)
    uint16_t window_size;       // Window Size
    uint16_t checksum;          // Checksum
    uint16_t urgent_pointer;    // Urgent Pointer (if URG flag is set)
};