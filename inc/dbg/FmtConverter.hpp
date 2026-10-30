#pragma once

#include "dbg/FmtStream.hpp"
#include "dbg/FmtUtils.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <typeindex>

namespace dbg {
    class FmtConverter {
    private:
        using Fn = std::function<FmtStream(const void*)>;
        std::unordered_map<std::type_index, Fn> table;

        mutable std::shared_mutex mmtx;

    public:
        FmtConverter() = default;
        FmtConverter(const FmtConverter&) = delete;
        FmtConverter& operator=(const FmtConverter&) = delete;

        static inline FmtStream fallback(
            std::type_index pType, const void* pData)
        {
            FmtStream ss;
            ss << Style::Yellow << "<" << Style::Red << Style::Bold
               << "unregistered type" << Style::Reset << Style::Yellow << ":"
               << Style::Reset << " " << dbg::getDemangledName(pType)
               << Style::Yellow << ">" << Style::Reset << " = " << Style::Bold
               << pData << Style::Reset;
            return ss;
        }

        template <typename T>
        void registerType(const std::function<FmtStream(const T&)>& fn)
        {
            using Clean = std::remove_cv_t<std::remove_reference_t<T>>;
            // remove cv = 'const' , 'volatile'
            // remove reference = & , &&

            std::unique_lock lock(mmtx);

            table[typeid(Clean)] = [fn](const void* p) -> FmtStream {
                return fn(*static_cast<const Clean*>(p));
            };
        }

        template <typename T> FmtStream toStream(const T& obj) const
        {
            using Clean = std::remove_cv_t<std::remove_reference_t<T>>;
            Fn fn;
            {
                std::shared_lock lock(mmtx);
                auto it = table.find(typeid(Clean));
                if (it != table.end()) {
                    fn = it->second;
                }
            }

            const void* p = std::addressof(obj);
            if (fn) {
                return fn(p);
            } else {
                return fallback(typeid(Clean), p);
            }
        }
    };
}