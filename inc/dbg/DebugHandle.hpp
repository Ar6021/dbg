#pragma once

#include "dbg/FmtConverter.hpp"
#include "dbg/FmtStream.hpp"
#include "dbg/LogString.hpp"
#include "dbg/Sinks.hpp"
#include "dbg/ThreadMap.hpp"
#include <memory>
#include <thread>

namespace dbg {

    class RegisterThreadInfo;

    class DebugHandle {
    public:
        friend class RegisterThreadInfo;

    protected:
        Sinks msinks;
        FmtConverter mcvt;
        LogString mlstr;
        ThreadMap mthreadMap { ThreadMap::defaultThreadName };

    public:
        DebugHandle()
        {
            msinks.registerSink(predefined::defaultStdoutSinkWithClr);
        };
        ~DebugHandle() = default;

        const Sinks& getSinks() { return msinks; }
        const FmtConverter& getConverter() { return mcvt; }
        const LogString& getLogString() { return mlstr; }

        void render(const FmtStream& pStream, const LogStringInfo& info)
        {
            msinks.render(
                mlstr.asString(
                    info, mthreadMap.getThreadInfo(std::this_thread::get_id())),
                pStream);
        }

        void renderNoInfo(const FmtStream& pStream)
        {
            msinks.render(std::string(), pStream);
        }

        template <typename T>
        void registerType(const std::function<FmtStream(const T&)>& fn)
        {
            mcvt.registerType<T>(fn);
        }
    };

    inline std::unique_ptr<DebugHandle> globalDebugHandle
        = std::make_unique<DebugHandle>();
}
