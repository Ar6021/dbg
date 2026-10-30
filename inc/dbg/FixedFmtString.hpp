#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <source_location>
#include <string_view>

#ifndef FMT_MAX_NAME
    #define FMT_MAX_NAME 128
#endif

namespace dbg {
    template <std::size_t N> struct FixedFmtString {
        char fmt[N];
        std::uint32_t line;
        char filename[FMT_MAX_NAME] { };
        char function_name[FMT_MAX_NAME] { };

        consteval FixedFmtString(const char (&pFmt)[N],
            std::source_location loc = std::source_location::current())
            : line(loc.line())
        {
            std::ranges::copy_n(pFmt, N, fmt);
            copy_trunc(filename, loc.file_name());
            copy_trunc(function_name, loc.function_name());
        }

        consteval void copy_trunc(char (&dst)[FMT_MAX_NAME], const char* src)
        {
            std::size_t i = 0;
            while (src[i] != 0 && i < FMT_MAX_NAME) {
                dst[i] = src[i];
                i++;
            }
            if (src[i] != 0) {
#ifndef REMOVE_EXPERIMENTAL
                static_assert("Overflow of name in NTTP!");
#endif // !REMOVE_EXPERIMENTAL
                dst[i - 3] = '.';
                dst[i - 2] = '.';
                dst[i - 1] = '.';
            }
            dst[i] = '\0';
        }

        consteval std::size_t size() const { return N; }

        consteval std::string_view view() const
        {
            return std::string_view(fmt, size());
        }
    };

    template <std::size_t N>
    FixedFmtString(const char (&)[N]) -> FixedFmtString<N>;
}