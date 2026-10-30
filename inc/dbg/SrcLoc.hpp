#pragma once

#include <cstdint>
#include <source_location>
#include <string>
#include <utility>
namespace dbg {
    struct SrcLoc {
        std::string filename;
        std::string functionName;
        std::uint32_t lineno;
    };

    inline SrcLoc toSrcLoc(std::source_location loc)
    {
        return { .filename = loc.file_name(),
            .functionName = loc.function_name(),
            .lineno = loc.line() };
    }

    inline SrcLoc toSrcLoc(const std::string& fname, const std::string& fnName,
        const std::uint32_t lineno)
    {
        return { .filename = std::move(fname),
            .functionName = std::move(fnName),
            .lineno = lineno };
    }
}