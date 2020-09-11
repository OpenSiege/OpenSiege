
#pragma once

#include <string>
#include <vector>

namespace ehb
{
    class StringTool
    {
    public:

        static std::vector<std::string> split(const std::string & value, char delim);

        static void trim(std::string & s);

        static bool sameNoCase(const std::string & lhs, const std::string & rhs);

        static bool isEmpty(const char * str);
    };
}
