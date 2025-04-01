#pragma once

#include <bit>
#include <memory>

#include "ldl_concepts.hpp"


namespace little_deserialization_library
{
    namespace reader_helpers
    {
        template<concepts::swappable_integral T> [[nodiscard]] constexpr T integral_swap (T t)
        {
            if constexpr (sizeof(T) == 2) {
                return T((t & T(0xFF00)) >> 8) | T(((t & T(0x00FF)) << 8));
            }
            else if constexpr (sizeof(T) == 4) {
                return T((t & T(0xFF000000)) >> 24) | T((t & T(0x00FF0000)) >> 8) | T((t & T(0x0000FF00)) << 8) | T((t & T(0x000000FF)) << 24);
            }
            else {
                return T((t & T(0xFF00000000000000LL)) >> 56) |
                       T((t & T(0x00FF000000000000LL)) >> 40) |
                       T((t & T(0x0000FF0000000000LL)) >> 24) |
                       T((t & T(0x000000FF00000000LL)) >> 8)  |
                       T((t & T(0x00000000FF000000LL)) << 8)  |
                       T((t & T(0x0000000000FF0000LL)) << 24) |
                       T((t & T(0x000000000000FF00LL)) << 40) |
                       T((t & T(0x00000000000000FFLL)) << 56);
            }
        }

        template<concepts::non_bool_integral T, std::floating_point F> requires(sizeof(T) == sizeof(F)) T to_integral (F f)
        {
            using byte_array = unsigned char *;

            T t;
            std::copy (std::bit_cast<byte_array> (&f), std::bit_cast<byte_array> (&f) + sizeof(F), std::bit_cast<byte_array> (&t));

            return t;
        }

        template<std::floating_point F, concepts::non_bool_integral T> requires(sizeof(T) == sizeof(F)) F to_floating_point (T t)
        {
            using byte_array = unsigned char *;

            F f;
            std::copy (std::bit_cast<byte_array> (&t), std::bit_cast<byte_array> (&t) + sizeof(T), std::bit_cast<byte_array> (&f));

            return f;
        }

        template<concepts::non_bool_integral T, std::floating_point F> requires(sizeof(T) == sizeof(F)) F floating_point_swap (F f)
        {
            return to_floating_point<F, T> (integral_swap (to_integral<T, F> (f)));
        }

        template<concepts::swappable_arithmetic T> T swap (T t)
        {
            if constexpr (std::is_floating_point_v<T> && (sizeof(T) == 2)) {
                return helpers::floating_point_swap<uint16_t> (t);
            }
            if constexpr (std::is_floating_point_v<T> && (sizeof(T) == 4)) {
                return helpers::floating_point_swap<uint32_t> (t);
            }
            if constexpr (std::is_floating_point_v<T> && (sizeof(T) == 8)) {
                return helpers::floating_point_swap<uint64_t> (t);
            }
            else {
                return helpers::integral_swap (t);
            }
        }

        template<concepts::non_bool_arithmetic T, concepts::byte_like B> constexpr T read_no_swap (const B * src)
        {
            T val;
            std::copy (src, src + sizeof(T), std::bit_cast<B *> (std::addressof (val)));

            return val;
        }
    }

    namespace reader
    {
        template<concepts::non_bool_arithmetic T, std::endian E = std::endian::big, concepts::byte_like B> constexpr T read (const B * src)
        {
            const auto val{reader_helpers::read_no_swap<T> (src)};
            if constexpr ((std::endian::native != E) && concepts::swappable_arithmetic<T>) {
                return reader_helpers::swap<E> (val);
            }
            else {
                return val;
            }
        }
    }
}
