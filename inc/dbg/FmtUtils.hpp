#pragma once

#include <memory>
#include <string>
#include <typeindex>

#if __has_include(<cxxabi.h>)
    #include <cxxabi.h>
    #define CXX_DEMANGLE_AVAIL 1
#else
    #define CXX_DEMANGLE_AVAIL 0
#endif

namespace dbg {
    inline std::string getDemangledName(std::type_index pType)
    {
#if CXX_DEMANGLE_AVAIL == 1
        int status;
        std::unique_ptr<char, void (*)(void*)> demangled {
            abi::__cxa_demangle(pType.name(), nullptr, nullptr, &status),
            std::free
        };
        std::string demangledName = status ? pType.name() : demangled.get();

        return demangledName;
#else
        return std::string(pType.name());
#endif
    }
}