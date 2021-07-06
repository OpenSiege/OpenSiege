
#pragma once

#include <string>

namespace ehb
{
    enum class JUSTIFICATION
    {
        justify_left = 0,
        justify_right = 1,
        justify_center = 2
    };

    bool fromString(const std::string & value, JUSTIFICATION & result);

    const char * toString(JUSTIFICATION value);
}
