#include <gtest/gtest.h>

#include "helpers/extended_mac_header.hpp"
#include "helpers/network_packets.hpp"
#include "helpers/utilities.hpp"

#include "ldl/object_deserializer.hpp"


namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<mac>
    {
        using type = std::tuple<std::array<uint8_t, mac::MAC_ADDRESS_LENGTH>>;
    };

    template<> struct rule<eth_header_composed>
    {
        using type = std::tuple<mac, mac, uint16_t>;
    };
}

// Simple deserialization and fields extraction
TEST(BuiltInArrayTest, DeserializeBuiltInArray) {

    namespace ldl = little_deserialization_library;

    ldl::network_packet_deserializer deserializer{std::span{eth_ip_tcp_packet}};
    auto ether_frame = deserializer.deserialize<eth_header_composed>();
    ASSERT_EQ(ether_frame.dest_mac, (mac{0x00, 0x11, 0x22, 0x33, 0x44, 0x55}));
    ASSERT_EQ(ether_frame.src_mac, (mac{0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}));
}