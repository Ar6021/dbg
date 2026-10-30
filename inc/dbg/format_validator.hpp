#pragma once

#include "dbg/DebugHandle.hpp"
#include "dbg/FixedFmtString.hpp"
#include "dbg/FmtStream.hpp"
#include <cstddef>
#include <functional>
namespace dbg {
    enum class PlaceholderStatus {
        NO_PLACEHOLDER = -1,
        FOUND_PLACEHOLDER = 0,
        CLOSING_BRACE_NOT_FOUND,
        OPENING_BRACE_NOT_FOUND,
    };

    using OptStream = std::optional<std::reference_wrapper<FmtStream>>;
    // let's glide with the ceremony...
    // like since it IS there, why not use it? ;)

    struct FmtValidationData {
        PlaceholderStatus status = PlaceholderStatus::FOUND_PLACEHOLDER;
        std::size_t placeholderCount = 0;
    };

    constexpr PlaceholderStatus consumeUntilPlaceholder(
        std::string_view& pCur, OptStream pStream)
    {
        std::size_t pos = 0;
        while ((pos = pCur.find_first_of("{}")) != std::string_view::npos) {
            char ch = pCur[pos];
            if (ch == '{') {
                if (pos > 0 && pStream) {
                    pStream->get() << pCur.substr(0, pos);
                }
                char ch1 = pCur[pos + 1]; // WILL WORK!
                if (ch1 == '}') {
                    pCur.remove_prefix(pos + 2);
                    return PlaceholderStatus::FOUND_PLACEHOLDER;
                } else if (ch1 == '{') {
                    // escape open curly
                    if (pStream) {
                        pStream->get() << '{';
                    }
                    pCur.remove_prefix(pos + 2);
                } else {
                    //
                    pCur.remove_prefix(pos + 2);
                    return PlaceholderStatus::CLOSING_BRACE_NOT_FOUND;
                }
            } else if (ch == '}') {
                if (pCur[pos + 1] == '}') {
                    if (pStream) {
                        pStream->get() << '}';
                    }
                    pCur.remove_prefix(pos + 2);
                } else {
                    pCur.remove_prefix(pos);
                    return PlaceholderStatus::OPENING_BRACE_NOT_FOUND;
                }
            }
        }

        if (pStream) {
            pStream->get() << pCur;
        }

        return PlaceholderStatus::NO_PLACEHOLDER;
    }

    template <FixedFmtString Fmt> consteval FmtValidationData validateFormat()
    {
        auto cur = Fmt.view();
        FmtValidationData res { };

        PlaceholderStatus s;
        while ((s = consumeUntilPlaceholder(cur, std::nullopt))
            != PlaceholderStatus::NO_PLACEHOLDER) {
            if (s != PlaceholderStatus::FOUND_PLACEHOLDER) {
                res.status = s;
                return res;
            }
            res.placeholderCount += 1;
        }
        return res;
    }

    template <FixedFmtString Fmt, typename... Args>
    void innerFormat(FmtStream& pStream, Args&&... pArgs)
    {
        constexpr auto stat = validateFormat<Fmt>();

        static_assert(stat.status != PlaceholderStatus::CLOSING_BRACE_NOT_FOUND,
            "Closing Brace not found!");
        static_assert(stat.status != PlaceholderStatus::OPENING_BRACE_NOT_FOUND,
            "Opening Brace not found!");
        static_assert(!(stat.placeholderCount < sizeof...(pArgs)),
            "More arguments than placeholders!");
        static_assert(!(stat.placeholderCount > sizeof...(pArgs)),
            "More placeholders than arguments!");

        auto cur = Fmt.view();

        // don't use a lambda variable, since that makes the
        // it have an unused-variable warning when pArgs is empty
        // the build output just looks ugly for no reason... :/
        (
            [&]<typename T>(T&& arg) {
                consumeUntilPlaceholder(cur, OptStream { pStream });
                if constexpr (FmtStreamable<T>) {
                    pStream << std::forward<T>(arg);
                } else {
                    pStream << globalDebugHandle->getConverter().toStream(
                        std::forward<T>(arg));
                }
            }(std::forward<Args>(pArgs)),
            ...);

        consumeUntilPlaceholder(cur, OptStream { pStream });
        // flush trailing literal text
    }

    template <typename... Args>
    void mergeToFormat(FmtStream& pStream, Args&&... pArgs)
    {
        (
            [&]<typename T>(T&& arg) {
                if constexpr (FmtStreamable<T>) {
                    pStream << std::forward<T>(arg);
                } else {
                    pStream << globalDebugHandle->getConverter().toStream(
                        std::forward<T>(arg));
                }
            }(std::forward<Args>(pArgs)),
            ...);
    }
}