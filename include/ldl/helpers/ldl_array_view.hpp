#pragma once

#include <span>
#include <array>

#include "ldl_concepts.hpp"


namespace little_deserialization_library
{
    template<concepts::byte_like B, size_t N> struct array_view
    {
        using array_ptr_type = B(*)[N];
        using array_ref_type = B(&)[N];

        explicit array_view(std::span<B, N> span) : span{ span } {}
        template<size_t M> requires(M >= N) explicit array_view(std::span<B, M> span) : span{ span.template subspan<0, N>() } {}

        explicit operator array_ptr_type (void) const { return reinterpret_cast<array_ptr_type>(span.data()); }
        explicit operator array_ref_type (void) const { return *reinterpret_cast<array_ptr_type>(span.data()); }
        template<typename B2 = B> explicit operator std::array<B2, N>(void) const { return to_array<B, N, B2>(span); }


        std::span<B, N> span;
    };

    template<typename T> using to_array_ref_t = std::conditional_t<is_bounded_byte_array<T>, std::add_lvalue_reference_t<T>, T>;
}
