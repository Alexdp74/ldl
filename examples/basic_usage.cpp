#include <cstdint>
#include <format>
#include <iostream>

#include "common/network_headers.hpp"
#include "ldl/object_deserializer.hpp"


namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<eth_header>
    { using type = std::tuple<std::array<uint8_t, 6U>, std::array<uint8_t, 6U>, uint16_t>; };

    template<> struct rule<ip_header>
    { using type = std::tuple<uint8_t, uint8_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint32_t, uint32_t>; };

    template<> struct rule<tcp_header>
    { using type = std::tuple<uint16_t, uint16_t, uint32_t, uint32_t, uint8_t, uint8_t, uint16_t, uint16_t, uint16_t>; };
}

std::string format_mac_address (std::array<uint8_t, 6U> mac) {
    return std::format ("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

std::string format_ip_address (uint32_t ip_address) {
    return std::format ("{}.{}.{}.{}", (ip_address >> 24) & 0xFF, (ip_address >> 16) & 0xFF, (ip_address >> 8) & 0xFF, ip_address & 0xFF);
}

int main()
{
    const uint8_t network_bytes[] = {
        // Ethernet Header (14 bytes)
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Destination MAC
        0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, // Source MAC
        0x08, 0x00,                         // EtherType: IPv4 (0x0800)

        // IPv4 Header (20 bytes)
        0x45, 0x00, 0x00, 0x34,             // Version, IHL, DSCP, ECN, Total Length (52 bytes)
        0x1A, 0x2B, 0x40, 0x00,             // Identification, Flags, Fragment Offset
        0x40, 0x06, 0x00, 0x00,             // TTL (64), Protocol (TCP), Header Checksum (Placeholder)
        0xC0, 0xA8, 0x01, 0x64,             // Source IP: 192.168.1.100
        0xC0, 0xA8, 0x01, 0x01,             // Destination IP: 192.168.1.1

        // TCP Header (20 bytes)
        0x30, 0x39, 0x00, 0x50,             // Source Port: 12345, Destination Port: 80
        0x12, 0x34, 0x56, 0x78,             // Sequence Number
        0x9A, 0xBC, 0xDE, 0xF0,             // Acknowledgment Number
        0x50, 0x02, 0x71, 0x10,             // Data Offset, Flags (SYN), Window Size
        0x00, 0x00, 0x00, 0x00,             // Checksum (Placeholder), Urgent Pointer

        // TCP Data (Optional, omitted)
    };

    namespace ldl = little_deserialization_library;
    auto packet{std::span{network_bytes}};
    ldl::NetworkPacketDeserializer deserializer{packet};
    auto ether_frame = deserializer.deserialize<eth_header>();
    auto ip_packet = deserializer.deserialize<ip_header>();
    auto tcp_packet = deserializer.deserialize<tcp_header>();

    std::cout << std::format ("Eth src: {} - Eth dest: {} - Eth type: {}",
                              format_mac_address (ether_frame.src_mac), format_mac_address (ether_frame.dest_mac), ether_frame.ethertype) << "\n";
    std::cout << std::format ("IP src: {} - IP dest: {} - IP Iden: {}",
                              format_ip_address (ip_packet.src_ip), format_ip_address (ip_packet.dest_ip), ip_packet.identification) << "\n";
    std::cout << std::format ("TCP src port: {} - TCP dest port: {} - TCP SEQ num: {}, TCP ACK num: {}",
                              tcp_packet.src_port, tcp_packet.dest_port, tcp_packet.seq_number, tcp_packet.ack_number);

    return 0;
}

// # build example:
// $ cmake --workflow --preset gcc-14