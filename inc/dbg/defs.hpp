#pragma once

#include <chrono>
#include <source_location>

namespace dbg {
    using StdSrcLoc = std::source_location;
    using SysClk = std::chrono::system_clock;
}