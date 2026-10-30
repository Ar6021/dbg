#pragma once

#include "dbg/Style.hpp"
#include "dbg/Text.hpp"
#include <sstream>
#include <vector>

template <typename T>
concept OStringStreamable
    = requires(std::ostringstream& pOss, const T& pVal) { pOss << pVal; };

namespace dbg {
    class FmtStream {
    private:
        std::vector<Text> texts;

    public:
        FmtStream() { }
        ~FmtStream() { }

        inline const std::vector<Text>& data() const { return texts; }

        inline void clear() { texts.clear(); }

        FmtStream& operator<<(const Style& pStyle)
        {
            texts.emplace_back(pStyle);
            return *this;
        }

        FmtStream& operator<<(const FmtStream& pStream)
        {
            texts.reserve(texts.size() + pStream.data().size());
            texts.insert(
                texts.end(), pStream.data().begin(), pStream.data().end());
            return *this;
        }

        template <OStringStreamable T> FmtStream& operator<<(const T& pVal)
        {
            std::ostringstream oss;
            oss << pVal;
            texts.emplace_back(Text { oss.str() });
            return *this;
        }
    };

    template <typename T>
    concept FmtStreamable = OStringStreamable<T> || std::same_as<T, FmtStream>
        || std::same_as<T, Style>;
}