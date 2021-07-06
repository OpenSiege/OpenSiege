
#include "NormalizedRect.hpp"

#include <cstdio>

namespace ehb
{
    bool fromString(const std::string& value, NormalizedRect& result)
    {
        return std::sscanf(value.c_str(), "%f,%f,%f,%f", &result.left, &result.top, &result.right, &result.bottom) == 4;
    }
}
