#pragma once

#include "dbg/ThreadInfo.hpp"
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace dbg {
    class ThreadMap {
    private:
        std::unordered_map<std::thread::id, ThreadInfo> mthreadInfoMap;
        mutable std::shared_mutex mmtx;

    public:
        static constexpr auto defaultThreadName = "main";

    public:
        ThreadMap(const std::string& defaultThread)
        {
            mthreadInfoMap[std::this_thread::get_id()] = ThreadInfo {
                .name = defaultThread,
            };
        };

        ThreadMap(const ThreadMap&) = delete;
        ThreadMap& operator=(const ThreadMap&) = delete;

        std::thread::id addInfo(
            const std::thread::id& tid, const ThreadInfo& info)
        {
            std::unique_lock lock(mmtx);
            mthreadInfoMap.try_emplace(tid, info);
            return tid;
        }

        void removeInfo(const std::thread::id& tid)
        {
            std::unique_lock lock(mmtx);
            mthreadInfoMap.erase(tid);
        }

        const ThreadInfo* getThreadInfo(const std::thread::id& tid) const
        {
            std::shared_lock lock(mmtx);
            if (auto it = mthreadInfoMap.find(tid);
                it != mthreadInfoMap.end()) {
                return &(it->second);
            } else {
                return nullptr;
            }
        }
    };
}