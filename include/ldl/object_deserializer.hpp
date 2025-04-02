#pragma once

#include <cstddef>
#include <bit>
#include <format>
#include <stdexcept>
#include <type_traits>

#include "helpers/ldl_array_view.hpp"
#include "helpers/ldl_concepts.hpp"
#include "helpers/ldl_deserialization_rules.hpp"
#include "helpers/ldl_reader.hpp"


namespace little_deserialization_library
{
    template<typename T> consteval size_t deserialization_length (void);
    template<typename T, std::endian E, concepts::byte_like B> requires(!is_any_array<T>) constexpr T deserialize (std::span<B> & packet);
    template<is_any_array T, std::endian E, concepts::byte_like B> constexpr auto deserialize (std::span<B> & packet);

    template<typename A> consteval auto array_size (void)
    {
        if constexpr (is_std_array<A>) {
            return std::tuple_size_v<A>;
        }
        else if constexpr (is_bounded_byte_array<A>) {
            return std::extent_v<A>;
        }
        else {
            static_assert(!std::is_same_v<A, A>, "the template type A is neither a c-style array nor a std::array");
        }
    }

    template<typename Tuple, size_t... Idx> constexpr size_t deserialization_length_from_tuple_impl (std::index_sequence<Idx...>)
    {
        return (deserialization_length<std::tuple_element_t<Idx, Tuple>>() + ...);
    }

    template<typename Tuple> constexpr size_t deserialization_length_from_tuple (void)
    {
        constexpr auto tuple_size{std::tuple_size_v<Tuple>};
        return deserialization_length_from_tuple_impl<Tuple> (std::make_index_sequence<tuple_size>());
    }

    template<typename T, typename Tuple, std::endian E, concepts::byte_like B, size_t... Idx>
        constexpr T construct_from_tuple_impl (std::span<B> & packet, std::index_sequence<Idx...>)
    {
        static_assert(concepts::aggregate_constructible<T, std::tuple_element_t<Idx, Tuple>...>, "Invalid deserialization rule");
        return T{static_cast<to_array_ref_t<std::tuple_element_t<Idx, Tuple>>>(deserialize<std::tuple_element_t<Idx, Tuple>, E> (packet))...};
    }

    template<typename T, typename Tuple, std::endian E, concepts::byte_like B> constexpr T construct_from_tuple (std::span<B> & packet)
    {
        constexpr auto tuple_size{std::tuple_size_v<Tuple>};
        return construct_from_tuple_impl<T, Tuple, E> (packet, std::make_index_sequence<tuple_size>());
    }

    template<typename T> consteval size_t deserialization_length (void)
    {
        if constexpr (concepts::non_bool_arithmetic<T>) {
            return sizeof(T);
        }
        else if constexpr (is_any_array<T>) {
            return array_size<T>();
        }
        else if constexpr (is_static_extent_byte_span<T>) {
            return T::extent;
        }
        else {
            return deserialization_length_from_tuple<deserialization_rules::rule_t<T>>();
        }
    }

    template<typename T, std::endian E, concepts::byte_like B> requires(!is_any_array<T>) constexpr T deserialize (std::span<B> & packet)
    {
        if constexpr (concepts::non_bool_arithmetic<T>) {
            const auto value{reader::read<T, E> (packet)};
            packet = packet.template subspan<sizeof(T)>();

            return value;
        }
        else if constexpr (is_static_extent_byte_span<T>) {
            static_assert(!std::is_const_v<B> || std::is_const_v<typename T::element_type>,
                          "cannot convert to a std::span over non-const bytes when the source is const");
            T value{packet.template subspan<0, T::extent>()};
            packet = packet.template subspan<T::extent>();

            return value;
        }
        else {
            return construct_from_tuple<T, deserialization_rules::rule_t<T>, E> (packet);
        }
    }

    template<is_any_array T, std::endian E, concepts::byte_like B> constexpr auto deserialize (std::span<B> & packet)
    {
        static_assert(is_std_array<T> || std::is_const_v<std::remove_extent_t<T>> || !std::is_const_v<B>,
                      "cannot convert to a non-const c-style byte array when the source is const");
        array_view view{packet.template subspan<0, array_size<T>()>()};
        packet = packet.template subspan<array_size<T>()>();

        return view;
    }

    template<concepts::byte_like B, std::endian E> class ObjectDeserializer
    {
    public:
        template<size_t N> explicit ObjectDeserializer (std::span<B, N> buffer) noexcept : buffer_{buffer} { }

        /// <summary>
        /// Constructs an object of type T with data in the buffer, possibly using a user-defined deserialization rule.
        /// Throws a std::length_error if the number of bytes available in the buffer is not enough to deserialize the object.
        /// </summary>
        /// <typeparam name="T">The type of the object to deserialize</typeparam>
        /// <returns>An instance of an object of type T, constructed from data read from the buffer</returns>
        template<typename T> T deserialize (void);
        /// <summary>
        /// Constructs an object of type T with data in the buffer, possibly using a user-defined deserialization rule.
        /// Skips length checks. The behavior is undefined if the buffer does not hold enough data to deserialize the object.
        /// </summary>
        /// <typeparam name="T">The type of the object to deserialize</typeparam>
        /// <returns>An instance of an object of type T, constructed from data read from the buffer</returns>
        template<typename T> T deserialize_noexcept (void) noexcept;
        /// <summary>
        /// Advances the buffer by the specified number of bytes.
        /// </summary>
        /// <param name="bytes">The number of bytes to skip in the buffer</param>
        constexpr void skip (size_t bytes);

        /// <summary>
        /// Returns a std::span over the first "size" bytes of the unread part of the buffer, or the whole buffer, whichever is smaller.
        /// </summary>
        /// <param name="size">The maximum number of bytes to include in the returned buffer</param>
        /// <returns>A std::span over at most the first "size" unread bytes in the buffer</returns>
        constexpr std::span<B> get_unread_buffer (size_t size = std::numeric_limits<size_t>::max()) const
        { return buffer_.first (std::min (buffer_.size(), size)); }

        /// <summary>
        /// Computes and returns the number of bytes that would be read from the buffer to deserialize an object of type T.
        /// </summary>
        /// <typeparam name="T">The type of the object</typeparam>
        /// <returns>The number of bytes that will be read to deserialize an object of type T</returns>
        template<typename T> static consteval auto deserialization_length (void) { return NOMADSUtil::deserialization_length<T>(); }


    private:
        std::span<B> buffer_;
    };

    template<concepts::byte_like B> using NetworkPacketDeserializer = ObjectDeserializer<B, std::endian::big>;

    template<concepts::byte_like B, std::endian E> template<typename T> inline T ObjectDeserializer<B, E>::deserialize (void)
    {
        if (static constexpr auto minimum_buffer_length{ObjectDeserializer::deserialization_length<T>()}; buffer_.size() < minimum_buffer_length) {
            throw std::length_error{std::format ("impossible to deserialize the requested object; Required bytes: {}; available bytes: {}",
                                                  minimum_buffer_length, buffer_.size())};
        }

        return deserialize_noexcept<T>();
    }

    template<concepts::byte_like B, std::endian E> template<typename T> inline T ObjectDeserializer<B, E>::deserialize_noexcept (void) noexcept
    {
        return NOMADSUtil::deserialize<T, E> (buffer_);
    }

    template<concepts::byte_like B, std::endian E> constexpr void ObjectDeserializer<B, E>::skip (size_t bytes)
    {
        if (buffer_.size() < bytes) {
            throw std::length_error{std::format ("impossible to skip {} bytes: available bytes {}", bytes, buffer_.size())};
        }

        buffer_ = buffer_.subspan (bytes);
    }
}
