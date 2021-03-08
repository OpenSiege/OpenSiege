
#pragma once

#include <string>
#include <vector>

namespace ehb
{
    // Dungeon Siege used WCHAR or wchar_t for some text strings,
    // which is 16bits wide on Windows. They are saved to file
    // as a pair of bytes so we need to use a properly sized type,
    // since the assumption that wchar_t is 16bits everywhere is
    // false. It is 32bits wide on Mac and Linux.
    using WideChar = char16_t;

    // Redefine std::string for the WideChar type.
    using WideString = std::basic_string<WideChar>;

    //! While this StringTool is antiquated it is used in Skrit so we shall abuse it
    class StringTool
    {
    public:

        static std::vector<std::string> split(const std::string & value, char delim);

        static void trim(std::string & s);

        static bool sameNoCase(const std::string & lhs, const std::string & rhs);

        static bool isEmpty(const char * str);

        static std::string wideStringToStdString(const WideString& wStr);

        static std::string toString(const WideString& wstr);

        static std::string removeTrailingFloatZeros(const std::string& floatStr);

        static std::string formatMemoryUnit(uint64_t memorySizeInBytes, bool abbreviated = false);
    };
}
