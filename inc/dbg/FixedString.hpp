#pragma once

#include <algorithm>
#include <cstddef>

namespace dbg {
    template <std::size_t N> struct FixedString {
        char data[N] { };
        consteval FixedString(const char (&str)[N])
        {
            std::ranges::copy_n(str, N, data);
        }
    };

    template <std::size_t N> FixedString(const char (&)[N]) -> FixedString<N>;

    template <typename T> struct is_fixed_string : std::false_type { };

    template <std::size_t N>
    struct is_fixed_string<FixedString<N>> : std::true_type { };

    template <typename T>
    concept IsFixedString = is_fixed_string<std::remove_cvref_t<T>>::value;
}