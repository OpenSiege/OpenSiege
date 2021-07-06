
#include "Rect.hpp"

#include <cstdio>

namespace ehb
{
    bool fromString(const std::string& value, Rect& result)
    {
        return std::sscanf(value.c_str(), "%d,%d,%d,%d", &result.left, &result.top, &result.right, &result.bottom) == 4;
    }
}
