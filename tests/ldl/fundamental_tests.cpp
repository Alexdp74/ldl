#include <gtest/gtest.h>

#include "helpers/network_headers.hpp"
#include "helpers/network_packets.hpp"
#include "helpers/utilities.hpp"

#include "ldl/object_deserializer.hpp"


namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<eth_header>
    {
        using type = std::tuple<std::array<uint8_t, 6U>, std::array<uint8_t, 6U>, uint16_t>;
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

// Simple deserialization and fields extraction
TEST(BasicDeserializationTest, FieldsExtraction) {

    namespace ldl = little_deserialization_library;

    auto packet{std::span{eth_ip_tcp_packet}};
    ldl::network_packet_deserializer deserializer{packet};
    auto ether_frame = deserializer.deserialize<eth_header>();
    ASSERT_EQ(ether_frame.dest_mac, std::to_array<uint8_t> ({0x00, 0x11, 0x22, 0x33, 0x44, 0x55}));
    ASSERT_EQ(ether_frame.src_mac, std::to_array<uint8_t> ({0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}));
    ASSERT_EQ(ether_frame.ethertype, 0x0800U);
    ASSERT_EQ(ether_frame.ethertype, 2048U);

    auto ip_packet = deserializer.deserialize<ip_header>();
    ASSERT_EQ(format_ip_address (ip_packet.src_ip), "192.168.1.100");
    ASSERT_EQ(format_ip_address (ip_packet.dest_ip), "192.168.1.1");
    ASSERT_EQ(ip_packet.identification, 6699U);
    ASSERT_EQ(ip_packet.ttl, 64U);

    auto tcp_packet = deserializer.deserialize<tcp_header>();
    ASSERT_EQ(tcp_packet.src_port, 12345U);
    ASSERT_EQ(tcp_packet.dest_port, 80U);
    ASSERT_EQ(tcp_packet.seq_number, 305419896U);
    ASSERT_EQ(tcp_packet.ack_number, 2596069104U);
}

// Simple deserialization assuming data is already in little endian (no big-endian --> little-endian conversion)
TEST(DeserializationLittleEndianTest, FieldsExtraction) {

    namespace ldl = little_deserialization_library;

    auto packet{std::span{eth_ip_tcp_packet}};
    ldl::object_deserializer<const unsigned char, std::endian::little> deserializer{packet};
    auto ether_frame = deserializer.deserialize<eth_header>();
    ASSERT_EQ(ether_frame.dest_mac, std::to_array<uint8_t>({0x00, 0x11, 0x22, 0x33, 0x44, 0x55}));
    ASSERT_EQ(ether_frame.src_mac, std::to_array<uint8_t>({0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}));
    ASSERT_EQ(ether_frame.ethertype, 0x0008U);
    ASSERT_EQ(ether_frame.ethertype, 8U);

    auto ip_packet = deserializer.deserialize<ip_header>();
    ASSERT_EQ(format_ip_address(ip_packet.src_ip), "100.1.168.192");
    ASSERT_EQ(format_ip_address(ip_packet.dest_ip), "1.1.168.192");
    ASSERT_EQ(ip_packet.identification, 11034U);    //0x2B 0x1A
    ASSERT_EQ(ip_packet.ttl, 64U);

    auto tcp_packet = deserializer.deserialize<tcp_header>();
    ASSERT_EQ(tcp_packet.src_port, 14640U);
    ASSERT_EQ(tcp_packet.dest_port, 20480U);
    ASSERT_EQ(tcp_packet.seq_number, 2018915346U);
    ASSERT_EQ(tcp_packet.ack_number, 4041129114U);
}

// Simple deserialization and fields extraction
TEST(BasicDeserializationTest, ParsingAndSkip) {

    namespace ldl = little_deserialization_library;

    constexpr auto packet{std::span{eth_ip_opt_tcp_seg_packet}};
    ldl::network_packet_deserializer deserializer{packet};
    deserializer.template skip<eth_header>();
    const auto ip_packet = deserializer.deserialize<ip_header>();
    ASSERT_EQ(format_ip_address(ip_packet.src_ip), "192.168.1.100");
    ASSERT_EQ(format_ip_address(ip_packet.dest_ip), "192.168.1.1");

    const auto ip_header_length{(ip_packet.ihl_version & 0x0F) * 4U};
    deserializer.skip (ip_header_length - deserializer.deserialization_length<ip_header>());
    auto tcp_packet = deserializer.deserialize<tcp_header>();
    ASSERT_EQ(tcp_packet.src_port, 12345U);
    ASSERT_EQ(tcp_packet.dest_port, 80U);
    ASSERT_EQ(tcp_packet.seq_number, 305419896U);
    ASSERT_EQ(tcp_packet.ack_number, 2596069104U);

    const auto tcp_segment{deserializer.get_unread_buffer()};
    std::string_view segment_data{std::bit_cast<const char*> (tcp_segment.data()), tcp_segment.size()};
    ASSERT_EQ(segment_data, "TEST");
}

// Constexpr
TEST(ConstexprFunctions, Constructor) {

    namespace ldl = little_deserialization_library;

    constexpr auto packet{std::span{eth_ip_opt_tcp_seg_packet}};
    constexpr ldl::network_packet_deserializer deserializer{packet};
    ASSERT_TRUE(true);
}
TEST(ConstexprFunctions, GetUnreadBuffer) {

    namespace ldl = little_deserialization_library;

    constexpr auto packet{std::span{eth_ip_opt_tcp_seg_packet}};
    constexpr ldl::network_packet_deserializer deserializer{packet};
    constexpr auto got_packet{deserializer.get_unread_buffer()};
    ASSERT_EQ(packet.data(), got_packet.data());
    ASSERT_EQ(got_packet.extent, std::dynamic_extent);
}