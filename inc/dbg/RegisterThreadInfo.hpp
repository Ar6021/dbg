#pragma once

#include "dbg/DebugHandle.hpp"
#include <thread>

namespace dbg {
    class RegisterThreadInfo {
    private:
        std::thread::id mthreadId;

    public:
        RegisterThreadInfo(const std::string& threadName)
            : mthreadId(std::this_thread::get_id())
        {
            globalDebugHandle->mthreadMap.addInfo(
                mthreadId, { .name = threadName });
        }

        ~RegisterThreadInfo()
        {
            globalDebugHandle->mthreadMap.removeInfo(mthreadId);
        }
    };
}