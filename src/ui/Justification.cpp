
#include "Justification.hpp"

namespace ehb
{
    bool fromString(const std::string & value, JUSTIFICATION & result)
    {
        if (value == "justify_left") result = JUSTIFICATION::justify_left;
        else if (value == "justify_right") result = JUSTIFICATION::justify_right;
        else if (value == "justify_center") result = JUSTIFICATION::justify_center;
        else return false;

        return true;
    }

    const char * toString(JUSTIFICATION value)
    {
        switch (value)
        {
            case JUSTIFICATION::justify_left: return "justify_left";
            case JUSTIFICATION::justify_right: return "justify_right";
            case JUSTIFICATION::justify_center: return "justify_center";
        }

        return nullptr;
    }
}
