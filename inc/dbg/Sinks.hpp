#pragma once

#include "dbg/FmtStream.hpp"
#include "dbg/Style.hpp"
#include <functional>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <variant>

namespace dbg {
    class Sinks {
    private:
        std::vector<std::function<void(const std::string&, const FmtStream&)>>
            sinkHandlers;

        mutable std::shared_mutex mmtx;

    public:
        Sinks() = default;
        Sinks(const Sinks&) = delete;
        ~Sinks() = default;

        void registerSink(
            std::function<void(const std::string&, const FmtStream&)> pFn)
        {
            std::unique_lock lock(mmtx);
            sinkHandlers.push_back(pFn);
        }

        int sinkCount() const noexcept
        {
            std::shared_lock lock(mmtx);
            return sinkHandlers.size();
        }

        void render(const std::string& pPrelude, const FmtStream& pStream) const
        {
            std::shared_lock lock(mmtx);
            for (const auto& handler : sinkHandlers) {
                handler(pPrelude, pStream);
            }
        }
    };
}

namespace dbg::predefined {
    inline void defaultStdoutSinkWithClr(
        const std::string& pPrelude, const FmtStream& pStream)
    {
        if (!pPrelude.empty()) {
            std::cout << pPrelude;
        }
        for (const auto& item : pStream.data()) {
            std::visit(
                [](auto&& it) {
                    using T = std::decay_t<decltype(it)>;
                    if constexpr (std::is_same_v<T, dbg::Style>) {
                        auto fmtStr = styleToANSI(it);
                        if (fmtStr.has_value()) {
                            std::cout << fmtStr.value();
                        }
                    } else {
                        std::cout << it;
                    }
                },
                item);
        }
    }
}