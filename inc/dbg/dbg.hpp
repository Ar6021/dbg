#pragma once

#include "dbg/DebugHandle.hpp"
#include "dbg/FixedFmtString.hpp"
#include "dbg/FmtStream.hpp"
#include "dbg/format_validator.hpp"

#include <utility>

namespace dbg {
    template <dbg::FixedFmtString Fmt, typename... Args>
    void printf(Args&&... args)
    {
#ifndef DBG_NDEBUG
        dbg::FmtStream fs;
        dbg::innerFormat<Fmt>(fs, std::forward<Args>(args)...);
        dbg::globalDebugHandle->renderNoInfo(fs);
#endif // !DBG_NDEBUG
    }

    template <dbg::FixedFmtString Fmt, typename... Args>
    void logf(Args&&... args)
    {
#ifndef DBG_NDEBUG
        dbg::FmtStream fs;
        dbg::innerFormat<Fmt>(fs, std::forward<Args>(args)...);
        dbg::globalDebugHandle->render(fs,
            { .loc = { .filename = Fmt.filename,
                  .functionName = Fmt.function_name,
                  .lineno = Fmt.line } });
#endif // !DBG_NDEBUG
    }

    template <dbg::FixedFmtString Fmt, typename... Args>
    void fmt(dbg::FmtStream& stream, Args&&... args)
    {
#ifndef DBG_NDEBUG
        dbg::innerFormat<Fmt>(stream, std::forward<Args>(args)...);
#endif // !DBG_NDEBUG
    }
}

#ifndef dbgprintf
    #ifndef DBG_NDEBUG
        #define dbgprintf(fmt, ...) (dbg::printf<fmt>(__VA_ARGS__))
    #else
        #define dbgprintf(fmt, ...)
    #endif // !DBG_NDEBUG
#endif // !dbgprintf

#ifndef dbgfmt
    #ifndef DBG_NDEBUG
        #define dbgfmt(fmt, stream, ...)                                       \
            (dbg::fmt<fmt>(stream __VA_OPT__(, ) __VA_ARGS__))
    #else
        #define dbgfmt(fmt, stream, ...)
    #endif // !DBG_NDEBUG
#endif // !dbgfmt

#ifndef dbglogf
    #ifndef DBG_NDEBUG
        #define dbglogf(fmt, ...) (dbg::logf<fmt>(__VA_ARGS__))
    #else
        #define dbglogf(fmt, ...)
    #endif // !DBG_NDEBUG
#endif // !dbglogf