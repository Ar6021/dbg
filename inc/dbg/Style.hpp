#pragma once

#include <optional>
#include <stdexcept>
#include <string>

namespace dbg {

    enum class Style {
        Reset,
        ResetFg,
        ResetBg,
        ResetBold,

        Bold,

        Red,
        Green,
        Yellow,
        Blue,
        Purple,
        Cyan,
        White,

        BgRed,
        BgGreen,
        BgYellow,
        BgBlue,
        BgPurple,
        BgCyan,
        BgWhite,
    };

    inline std::optional<std::string> styleToANSI(Style style)
    {
        switch (style) {
        case Style::Reset:
            return "\x1b[0m";
        case Style::ResetFg:
            return "\x1b[39m";
        case Style::ResetBg:
            return "\x1b[49m";
        case Style::ResetBold:
            return "\x1b[22m";

        case Style::Bold:
            return "\x1b[1m";

        case Style::Red:
            return "\x1b[91m";
        case Style::Green:
            return "\x1b[92m";
        case Style::Yellow:
            return "\x1b[93m";
        case Style::Blue:
            return "\x1b[94m";
        case Style::Purple:
            return "\x1b[95m";
        case Style::Cyan:
            return "\x1b[96m";
        case Style::White:
            return "\x1b[97m";

        case Style::BgRed:
            return "\x1b[101m";
        case Style::BgGreen:
            return "\x1b[102m";
        case Style::BgYellow:
            return "\x1b[103m";
        case Style::BgBlue:
            return "\x1b[104m";
        case Style::BgPurple:
            return "\x1b[105m";
        case Style::BgCyan:
            return "\x1b[106m";
        case Style::BgWhite:
            return "\x1b[107m";
        }

        return { };
    }

    class OutOfSyncStyleToANSI : public std::runtime_error {
    public:
        explicit OutOfSyncStyleToANSI(Style pStyle)
            : std::runtime_error(
                  "Style enum has no corresponding ANSI conversion: "
                  + std::to_string(static_cast<int>(pStyle)))
        {
        }
    };
}