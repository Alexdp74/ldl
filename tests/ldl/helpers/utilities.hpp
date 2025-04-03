#pragma once
#include <cstdint>
#include <string>
#include <format>


inline std::string format_ip_address (uint32_t ip_address)
{
    return std::format ("{}.{}.{}.{}", (ip_address >> 24) & 0xFF, (ip_address >> 16) & 0xFF, (ip_address >> 8) & 0xFF, ip_address & 0xFF);
}