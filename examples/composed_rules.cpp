#include <cstdint>
#include <format>
#include <iostream>

#include "common/network_headers.hpp"
#include "ldl/object_deserializer.hpp"


namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<mac>
    {
        using type = std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>;
    };

    template<> struct rule<eth_header_composed>
    {
        using type = std::tuple<mac, mac, uint16_t>;
    };
}

std::string format_mac_address (uint8_t mac[6]) {
    return std::format("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

std::string format_ip_address (uint32_t ip_address) {
    return std::format("{}.{}.{}.{}", (ip_address >> 24) & 0xFF, (ip_address >> 16) & 0xFF, (ip_address >> 8) & 0xFF, ip_address & 0xFF);
}

int main()
{
    const uint8_t network_bytes[] = {
        // Ethernet Header (14 bytes)
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Destination MAC
        0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, // Source MAC
        0x08, 0x00                          // EtherType: IPv4 (0x0800)
    };

    namespace ldl = little_deserialization_library;
    auto packet{std::span{network_bytes}};
    ldl::NetworkPacketDeserializer deserializer{packet};
    auto ether_frame = deserializer.deserialize<eth_header_composed>();

    std::cout << std::format ("Eth src: {} - Eth dest: {} - Eth type: {}", format_mac_address (ether_frame.src_mac.address),
                              format_mac_address (ether_frame.dest_mac.address), ether_frame.ethertype) << "\n";

    return 0;
}

// # build example:
// $ cmake --workflow --preset gcc-14