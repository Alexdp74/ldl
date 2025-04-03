#include <gtest/gtest.h>

#include "helpers/network_headers.hpp"
#include "helpers/network_packets.hpp"
#include "helpers/utilities.hpp"

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

    template<> struct rule<ip_header>
    {
        using type = std::tuple<uint8_t, uint8_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint32_t, uint32_t>;
    };

    template<> struct rule<tcp_header>
    {
        using type = std::tuple<uint16_t, uint16_t, uint32_t, uint32_t, uint8_t, uint8_t, uint16_t, uint16_t, uint16_t>;
    };
}

// Deserialization through composed rules
TEST(ComposedDeserializationTest, FieldsExtraction) {

    namespace ldl = little_deserialization_library;

    auto packet{std::span{eth_ip_tcp_packet}};
    ldl::network_packet_deserializer deserializer{packet};
    auto ether_frame = deserializer.deserialize<eth_header_composed>();
    ASSERT_EQ(ether_frame.dest_mac, (mac{0x00, 0x11, 0x22, 0x33, 0x44, 0x55}));
    ASSERT_EQ(ether_frame.src_mac, (mac{0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}));
}