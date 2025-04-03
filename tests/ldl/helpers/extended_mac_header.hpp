#pragma once

#include <cstdint>
#include <array>
#include <span>


// Ethernet Frame Header
struct mac
{
    static constexpr size_t MAC_ADDRESS_LENGTH{6U};

    constexpr explicit mac (const uint8_t (&mac)[MAC_ADDRESS_LENGTH]) noexcept
    {
        [this, mac]<size_t... Is> (std::index_sequence<Is...>)
        {
            ((this->address[Is] = mac[Is]), ...);
        } (std::make_index_sequence<MAC_ADDRESS_LENGTH>());
    }
    constexpr explicit mac (std::span<const uint8_t, MAC_ADDRESS_LENGTH> mac) noexcept
    {
        [this, mac]<size_t... Is> (std::index_sequence<Is...>)
        {
            ((this->address[Is] = mac[Is]), ...);
        } (std::make_index_sequence<MAC_ADDRESS_LENGTH>());
    }
    constexpr explicit mac (std::array<uint8_t, MAC_ADDRESS_LENGTH> mac) noexcept
    {
        [this, mac] <size_t... Is> (std::index_sequence<Is...>)
        {
            ((this->address[Is] = mac[Is]), ...);
        } (std::make_index_sequence<MAC_ADDRESS_LENGTH>());
    }
    template<typename... B> requires(sizeof...(B) == MAC_ADDRESS_LENGTH) mac (B... bytes) noexcept : address{static_cast<uint8_t>(bytes)...} { }

    [[nodiscard]] bool friend operator== (const mac & lhs, const mac & rhs) noexcept
    {
        return [lhs, rhs]<size_t... Is> (std::index_sequence<Is...>) {
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