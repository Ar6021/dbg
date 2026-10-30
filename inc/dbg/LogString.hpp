#pragma once

#include "dbg/FixedString.hpp"
#include "dbg/LogFormatType.hpp"
#include "dbg/SrcLoc.hpp"
#include "dbg/ThreadInfo.hpp"
#include "dbg/defs.hpp"

#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace dbg {

    using LogStringFormat = std::variant<LogFormatType, std::string>;

    template <typename T>
    concept IsLogFormatType
        = std::same_as<std::remove_cvref_t<T>, LogFormatType>;

    template <typename T>
    concept LogFormatOrFixedString = IsLogFormatType<T> || IsFixedString<T>;

    struct LogStringInfo {
        SrcLoc loc;
        SysClk::time_point pt = SysClk::now();
    };

    class LogString {
    public:
        using FmtVec = std::vector<LogStringFormat>;

    private:
        template <auto V> static consteval LogStringFormat m_sanitize()
        {
            if constexpr (IsFixedString<decltype(V)>) {
                return LogStringFormat(std::in_place_type<std::string>, V.data);
            } else {
                return LogStringFormat(std::in_place_type<LogFormatType>, V);
            }
        }
        static void m_defaultFill(FmtVec& v)
        {
            v.reserve(9);
            v.emplace_back(std::in_place_type<std::string>, "[");
            v.emplace_back(
                std::in_place_type<LogFormatType>, LogFormatType::TIME_DEFAULT);
            v.emplace_back(std::in_place_type<std::string>, "] @ ");
            v.emplace_back(std::in_place_type<LogFormatType>,
                LogFormatType::FUNCTION_NAME);
            v.emplace_back(std::in_place_type<std::string>, " >> ");
            v.emplace_back(std::in_place_type<LogFormatType>,
                LogFormatType::SOURCE_FILENAME);
            v.emplace_back(std::in_place_type<std::string>, ":");
            v.emplace_back(
                std::in_place_type<LogFormatType>, LogFormatType::LINE_NUMBER);
            v.emplace_back(std::in_place_type<std::string>, " => ");
        }
        template <auto... Us> static FmtVec m_build()
        {
            FmtVec v;
            if constexpr (sizeof...(Us) == 0) {
                m_defaultFill(v);
            } else {
                v.reserve(sizeof...(Us));
                (v.push_back(m_sanitize<Us>()), ...);
            }
            return v;
        }

        FmtVec mfmt;

    public:
        LogString() { m_defaultFill(mfmt); }

        template <auto... Vs>
            requires(LogFormatOrFixedString<decltype(Vs)> && ...)
        void set()
        {
            mfmt.assign({ m_sanitize<Vs>()... });
        }

        void setDefault()
        {
            FmtVec().swap(mfmt);
            m_defaultFill(mfmt);
        }

        const FmtVec& getFormat() const noexcept { return mfmt; }

        std::string asString(
            const LogStringInfo& lsi, const ThreadInfo* tinfo) const noexcept
        {
            std::string str;
            for (auto& f : mfmt) {
                std::visit(
                    [&str, &lsi, &tinfo](auto& val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            str += val;
                        } else {
                            auto s = logFormatTypeAsString(
                                val, lsi.pt, lsi.loc, tinfo);
                            str += s.value_or("(null)");
                        }
                    },
                    f);
            }
            return str;
        }
    };
}