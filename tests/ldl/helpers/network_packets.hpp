#pragma once

const uint8_t eth_ip_tcp_packet[] = {
    // Ethernet Header (14 bytes)
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Destination MAC
    0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, // Source MAC
    0x08, 0x00,                         // EtherType: IPv4 (0x0800)

    // IPv4 Header (20 bytes)
    0x45, 0x00, 0x00, 0x34,             // Version, IHL, DSCP, ECN, Total Length (52 bytes)
    0x1A, 0x2B, 0x40, 0x00,             // Identification: 6699, Flags, Fragment Offset
    0x40, 0x06, 0x00, 0x00,             // TTL (64), Protocol (TCP), Header Checksum (Placeholder)
    0xC0, 0xA8, 0x01, 0x64,             // Source IP: 192.168.1.100
    0xC0, 0xA8, 0x01, 0x01,             // Destination IP: 192.168.1.1

    // TCP Header (20 bytes)
    0x30, 0x39, 0x00, 0x50,             // Source Port: 12345, Destination Port: 80
    0x12, 0x34, 0x56, 0x78,             // Sequence Number: 305419896
    0x9A, 0xBC, 0xDE, 0xF0,             // Acknowledgment Number: 2596069104
    0x50, 0x02, 0x71, 0x10,             // Data Offset, Flags (SYN), Window Size
    0x00, 0x00, 0x00, 0x00,             // Checksum (Placeholder), Urgent Pointer
};

const uint8_t eth_ip_opt_tcp_seg_packet[] = {
    // Ethernet Header (14 bytes)
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Destination MAC
    0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, // Source MAC
    0x08, 0x00,                         // EtherType: IPv4 (0x0800)

    // IPv4 Header with Options (24 bytes)
    0x46, 0x00, 0x00, 0x3C,             // Version (4), IHL (6 -> 24-byte header), Total Length (60 bytes)
    0x1A, 0x2B, 0x40, 0x00,             // Identification: 6699, Flags, Fragment Offset
    0x40, 0x06, 0x00, 0x00,             // TTL (64), Protocol (TCP), Header Checksum (Placeholder)
    0xC0, 0xA8, 0x01, 0x64,             // Source IP: 192.168.1.100
    0xC0, 0xA8, 0x01, 0x01,             // Destination IP: 192.168.1.1

    // IP Options (4 bytes: NOP, NOP, Security option)
    0x01, 0x01, 0x82, 0x00,

    // TCP Header (20 bytes)
    0x30, 0x39, 0x00, 0x50,             // Source Port: 12345, Destination Port: 80
    0x12, 0x34, 0x56, 0x78,             // Sequence Number: 305419896
    0x9A, 0xBC, 0xDE, 0xF0,             // Acknowledgment Number: 2596069104
    0x50, 0x18, 0x71, 0x10,             // Data Offset (5), Flags (ACK), Window Size
    0x00, 0x00, 0x00, 0x00,             // Checksum (Placeholder), Urgent Pointer

    // TCP Data (4 bytes: "TEST")
    0x54, 0x45, 0x53, 0x54
};