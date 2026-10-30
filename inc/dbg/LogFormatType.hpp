#pragma once

#include "dbg/SrcLoc.hpp"
#include "dbg/ThreadInfo.hpp"
#include "dbg/defs.hpp"
#include <bits/chrono.h>
#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
namespace dbg {
    enum class LogFormatType {

        THREAD,

        SOURCE_FILENAME,
        LINE_NUMBER,
        FUNCTION_NAME,

        TIME_DAY_NUM,
        TIME_MON_NUM,
        TIME_YEAR_FULL,
        TIME_HH,
        TIME_MM,
        TIME_SS,
        TIME_MS,

        TIME_DEFAULT,
    };

    inline std::optional<std::string> logFormatTypeAsString(
        const LogFormatType ptype, const SysClk::time_point& pt,
        const SrcLoc& sloc, const ThreadInfo* tinfo)
    {
        using namespace std::chrono;
        std::stringstream ss;
        const auto localpt = zoned_time { current_zone(), pt }.get_local_time();
        const auto today = floor<days>(localpt);
        const year_month_day date(today);
        const hh_mm_ss time(localpt - today);
        const auto now_millis = floor<milliseconds>(localpt);

        ss << std::setfill('0') << std::setw(2);

        switch (ptype) {
        case LogFormatType::THREAD:
            ss << std::setw(0);
            if (tinfo != nullptr) {
                ss << tinfo->name;
            } else {
                ss << std::this_thread::get_id();
            }
            break;
        case LogFormatType::SOURCE_FILENAME:
            return sloc.filename;
        case LogFormatType::LINE_NUMBER:
            return std::to_string(sloc.lineno);
        case LogFormatType::FUNCTION_NAME:
            return sloc.functionName;
        case LogFormatType::TIME_DAY_NUM:
            ss << date.day();
            break;
        case LogFormatType::TIME_MON_NUM:
            ss << date.month();
            break;
        case LogFormatType::TIME_YEAR_FULL:
            ss << std::setw(4) << date.year();
            break;
        case LogFormatType::TIME_HH:
            ss << time.hours().count();
            break;
        case LogFormatType::TIME_MM:
            ss << time.minutes().count();
            break;
        case LogFormatType::TIME_SS:
            ss << time.seconds().count();
            break;
        case LogFormatType::TIME_MS:
            ss << std::setw(3)
               << duration_cast<milliseconds>(time.subseconds()).count();
            break;
        case LogFormatType::TIME_DEFAULT:
            ss << std::format("{:%F %T}", now_millis);
            break;
        default:
            return { };
        }

        return ss.str();
    }
}