# ldl
The Little Deserialization Library (LDL) is a C++20 header-only library that enables the deserialization and construction of objects from byte arrays.

## Usage
Three parts are involved in the deserialization of an object:
1. The struct or class definition of the object to instantiate;
2. The data from which to instantiate the object, in the form of a `std::span<B, N>`, where B is one among (possibly cv-qualified) `char`, `unsigned char`, and `std::byte`;
3. A deserialization rule, which instructs the library on which fields to extract to call the constructor of the object.

Here is a simple example:
```
// File: network_headers.hpp
#include <cstdint>

// IPv4 Header
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
```

```
// File: ldl_deserialization_rules.hpp
#include <ldl/object_deserializer.hpp">

#include "network_headers.hpp"    // For struct ip_header

// Defines rules to deserialize objects
namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<ip_header>
    { using type = std::tuple<uint8_t, uint8_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t, uint32_t, uint32_t>; };
}
```

```
// File: ldl_example.cpp
#include <iostream>
#include <format>

#include "ldl_deserialization_rules.hpp"

int main()
{
    // Queue of network packets, encoded is in big-endian
    const auto packet = packet_queue.dequeue();

    namespace ldl = little_deserialization_library;
    ldl::object_deserializer<const uint8_t, std::endian::big> deserializer{std::span{packet.data(), packet.size()}};
    try {
        auto ip_packet = deserializer.deserialize<ip_header>();
        std::cout << std::format ("IP src: {} - IP dest: {} - IP Iden: {}",
                                  ip_packet.src_ip, ip_packet.dest_ip, ip_packet.identification) << "\n";
    }
    catch (const std::length_error &) {
        std::cerr << "Not enough bytes in the buffer to deserialize an IP header, which requires at least " << 
                     deserializer.deserialization_length<ip_header>() << " bytes.\n";
    }

    return 0;
}
```

Rules can be composed recursively:
```
// File: network_headers.hpp
#include <cstdint>
#include <utility>

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


    uint8_t address[MAC_ADDRESS_LENGTH];   // MAC Address
};

struct eth_header_composed
{
    mac dest_mac;           // Destination MAC Address
    mac src_mac;            // Source MAC Address
    uint16_t ethertype;     // EtherType (e.g., 0x0800 for IPv4)
};
```

```
// File: ldl_composed_deserialization_rules.hpp
#include <ldl/object_deserializer.hpp">

#include "network_headers.hpp"    // For struct eth_header_composed, mac

// Defines rules to deserialize objects
namespace little_deserialization_library::deserialization_rules
{
    template<> struct rule<mac>
    {
        using type = std::tuple<const unsigned char[mac::MAC_ADDRESS_LENGTH]>;
    };

    template<> struct rule<eth_header_composed>
    {
        using type = std::tuple<mac, mac, uint16_t>;
    };
}
```

```
// File: ldl_compose_example.cpp
#include <iostream>
#include <format>

#include "ldl_composed_deserialization_rules.hpp"

int main()
{
    const uint8_t network_bytes[] = {
        // Ethernet frame + IP packet + Transport + payload
        ...
    };

    namespace ldl = little_deserialization_library;
    ldl::network_packet_deserializer deserializer{std::span{network_bytes}};    // alias for template<byte-like B> object_deserializer<B, std::endian::big>
    auto eth_frame = deserializer.deserialize<eth_header_composed>();
    
    std::cout << std::format ("Eth src: {} - Eth dest: {} - Eth type: {}",
                              mac_to_str (eth_frame.src_mac), mac_to_str (eth_frame.dest_mac), eth_frame.ethertype) << "\n";

    return 0;
}

```

Deserialization rules are expressed by specializing the template struct `little_deserialization_library::deserialization_rules::rule<T>` for the target object types.
A valid rule defines a `type` alias that expresses a `std::tuple` of the parameters to extract from the source bytes.

## Features
- Deserialization of primitive types from a contiguous byte region, passed in as a `std::span`.
- Deserialization of `std::span<B, N>`, `std::array<B, N>`, and built-in arrays in the form `B[N]` from a contiguous byte region; `B` must satisfy the `byte-like` concept, defined as:
  ```
  template<typename T> concept byte_like = std::is_same_v<std::remove_cv_t<T>, char> ||
                                           std::is_same_v<std::remove_cv_t<T>, unsigned char> ||
                                           std::is_same_v<std::remove_cv_t<T>, std::byte>;
  ```
- Supports both big-endian and little-endian through template non-type parameters.
- Deserialization of user-defined types through the definition of deserialization rules.
- Deserialization rule composition, to deserialize nested user-defined types.
- Compile-time calculation of the number of bytes required to deserialize an object of a given type.

## Future Goals
- Skip bytes as part of deserialization rules;
- Construct objects using parameters in part deserialized from the byte array and in part passed in as argument of the `deserialize<T>()` call;
- Add benchmarks.
