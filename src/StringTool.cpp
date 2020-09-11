
#include "StringTool.hpp"

#include <algorithm>
#include <sstream>
#include <cctype>

namespace ehb
{
    std::vector<std::string> StringTool::split(const std::string & value, char delim)
    {
        std::vector<std::string> result;

        std::istringstream stream(value);

        while (!stream.eof())
        {
            std::string item;
            std::getline(stream, item, delim);

            result.push_back(item);
        }

        return result;
    }

    void StringTool::trim(std::string &s)
    {
        s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](char c) { return std::isspace(c); }));
        s.erase(std::find_if_not(s.rbegin(), s.rend(), [](char c) { return std::isspace(c); }).base(), s.end());
    }

    bool StringTool::sameNoCase(const std::string & lhs, const std::string & rhs)
    {
        return ( (lhs.size() == rhs.size() ) &&
            std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](const char & c1, const char & c2) {
                if (c1 == c2) return true;
                if (std::toupper(c1) == std::toupper(c2)) return true;
                return false;
                }) );
    }

    bool StringTool::isEmpty(const char * str)
    {
        return std::string(str).empty();
    }
}
