#pragma once

#include "dbg/Style.hpp"
#include <string>
#include <variant>

namespace dbg {

    using Text = std::variant<std::string, Style>;

}