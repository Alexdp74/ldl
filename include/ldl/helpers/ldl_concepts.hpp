#pragma once

#include <cstddef>
#include <concepts>
#include <type_traits>


namespace little_deserialization_library::concepts
{
    /// <summary>
    /// Requires that T is a bool.
    /// </summary>
    template<typename T> concept boolean = std::is_same_v<T, bool>;

    /// <summary>
    /// Requires that T is one among (possibly cv-qualified) char, unsigned char, and std::byte.
    /// </summary>
    template<typename T> concept byte_like = std::is_same_v<std::remove_cv_t<T>, char> ||
                                             std::is_same_v<std::remove_cv_t<T>, unsigned char> ||
                                             std::is_same_v<std::remove_cv_t<T>, std::byte>;

    /// <summary>
    /// Requires that T is an integral type of size 2, 4, or 8 bytes.
    /// </summary>
    template<typename T> concept swappable_integral = std::is_integral_v<T> && ((sizeof(T) == 2) || (sizeof(T) == 4) || (sizeof(T) == 8));

    /// <summary>
    /// Requires that T is either a floating point or satisfies the swappable_integral concept.
    /// </summary>
    template<typename T> concept swappable_arithmetic = std::is_floating_point_v<T> || swappable_integral<T>;

    /// <summary>
    /// Requires that T is of any arithmetic type.
    /// </summary>
    template<typename T> concept arithmetic = std::integral<T> || std::floating_point<T>;

    /// <summary>
    /// Requires that T is of any integral type, except bool.
    /// </summary>
    template<typename T> concept non_bool_integral = std::integral<T> && !std::is_same_v<std::remove_cvref_t<T>, bool>;

    /// <summary>
    /// Requires that T is of any arithmetic type, except bool.
    /// </summary>
    template<typename T> concept non_bool_arithmetic = non_bool_integral<T> || std::floating_point<T>;

    /// <summary>
    /// Requires that TO is brace-constructible from FROM (TO{from} does not allow narrowing).
    /// </summary>
    template<typename FROM, typename TO> concept not_narrowing = requires (FROM from) { TO{from}; };

    /// <summary>
    /// Requires that T is bracket-constructible from Args.
    /// </summary>
    template<typename T, typename... Args> concept aggregate_constructible = std::constructible_from<T, Args...> ||
                                                                             requires (Args... args) { T{ args... }; };

    /// <summary>
    /// Requires that A is a specialization of std::array.
    /// To do so, it checks that std::tuple_size<A> and std::tuple_element_t<0, A> are defined and that
    /// std::is_same_v<A, std::array<std::tuple_element_t<0, A>, std::tuple_size_v<A>>> is true.
    /// </summary>
    template<typename A> concept is_std_array = requires(A)
    {
        typename std::tuple_size<A>::type;
        typename std::tuple_element_t<0, A>;
        requires std::is_same_v<A, std::array<std::tuple_element_t<0, A>, std::tuple_size_v<A>>>;
    };

    /// <summary>
    /// Requires that A is a bounded built-in array of rank 1.
    /// </summary>
    template<typename A> concept is_bounded_byte_array = std::is_bounded_array_v<A> && (std::rank_v<A> == 1U) &&
                                                         concepts::byte_like<std::remove_all_extents_t<A>>;

    /// <summary>
    /// Requires that A is either a specialization of std::array or a bounded built-in array.
    /// </summary>
    template<typename A> concept is_any_array = is_std_array<A> || is_bounded_byte_array<A>;

    /// <summary>
    /// Requires that S is a specialization of std::span with a static extent and a byte-like element_type.
    /// </summary>
    template<typename S> concept is_static_extent_byte_span = std::is_same_v<S, std::span<typename S::element_type, S::extent>> &&
                                                              concepts::byte_like<typename S::element_type> && (S::extent != std::dynamic_extent);
}
