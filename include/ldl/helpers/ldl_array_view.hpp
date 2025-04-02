#pragma once

#include <span>
#include <array>
#include <utility>

#include "ldl_concepts.hpp"


namespace little_deserialization_library
{
    namespace array_view_helpers
    {
        template<typename T1, std::size_t N, typename T2 = T1, size_t M = N> requires(M != std::dynamic_extent) auto to_array(std::span<T1, N> span)
        {
            return[span]<std::size_t... Is>(std::index_sequence<Is...>) { return std::array<T2, M>{T2(span[Is])...}; } (std::make_index_sequence<M>());
        }
    }

    template<concepts::byte_like B, size_t N> struct array_view
    {
        using array_ptr_type = B(*)[N];
        using array_ref_type = B(&)[N];

        explicit array_view(std::span<B, N> span) : span{ span } {}
        template<size_t M> requires(M >= N) explicit array_view(std::span<B, M> span) : span{ span.template subspan<0, N>() } {}

        explicit operator array_ptr_type (void) const { return reinterpret_cast<array_ptr_type>(span.data()); }
        explicit operator array_ref_type (void) const { return *reinterpret_cast<array_ptr_type>(span.data()); }
        template<typename B2 = B> explicit operator std::array<B2, N>(void) const { return array_view_helpers::to_array<B, N, B2>(span); }


        std::span<B, N> span;
    };

    template<typename T> using to_array_ref_t = std::conditional_t<concepts::is_bounded_byte_array<T>, std::add_lvalue_reference_t<T>, T>;
}
