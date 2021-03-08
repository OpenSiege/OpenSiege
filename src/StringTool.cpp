
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

	std::string StringTool::wideStringToStdString(const WideString& wStr)
	{
		static_assert(sizeof(WideChar) == 2, "This will only work if we are dealing with 2-byte wchars!");

		//
		// Convert 2-byte long Windows wchar_t string to a C string.
		//
		// Currently not doing a proper conversion, just grabbing
		// the lower byte of each WideChar.
		//
		// Also uses a fixed size buffer, so string length is
		// limited to MaxTempStringLen!
		//

		if (wStr.empty())
		{
			return std::string();
		}

		int i;
		char temBuf[2048];

		for (i = 0; i < (2048 - 1); ++i)
		{
			const char c = static_cast<char>(wStr[i] & 0x00FF);
			temBuf[i] = c;
			if (c == 0)
			{
				break;
			}
		}

		if (i == (2048 - 1))
		{
			return {};
		}

		temBuf[i] = '\0';
		return temBuf;
	}

	std::string StringTool::toString(const WideString& wStr)
	{
		return wStr.empty() ? "<EMPTY>" : ("\"" + wideStringToStdString(wStr) + "\"");
	}

	std::string StringTool::removeTrailingFloatZeros(const std::string& floatStr)
	{
		// Only process if the number is decimal (has a dot somewhere):
		if (floatStr.find_last_of('.') == std::string::npos)
		{
			return floatStr;
		}

		std::string trimmed(floatStr);

		// Remove trailing zeros:
		while (!trimmed.empty() && (trimmed.back() == '0'))
		{
			trimmed.pop_back();
		}

		// If the dot was left alone at the end, remove it too:
		if (!trimmed.empty() && (trimmed.back() == '.'))
		{
			trimmed.pop_back();
		}

		return trimmed;
	}

	std::string StringTool::formatMemoryUnit(uint64_t memorySizeInBytes, bool abbreviated)
	{
		const char* memUnitStr;
		double adjustedSize;
		char numStrBuf[128];

		if (memorySizeInBytes < 1024)
		{
			memUnitStr = (abbreviated ? "B" : "Bytes");
			adjustedSize = static_cast<double>(memorySizeInBytes);
		}
		else if (memorySizeInBytes < (1024 * 1024))
		{
			memUnitStr = (abbreviated ? "KB" : "Kilobytes");
			adjustedSize = (memorySizeInBytes / 1024.0);
		}
		else if (memorySizeInBytes < (1024 * 1024 * 1024))
		{
			memUnitStr = (abbreviated ? "MB" : "Megabytes");
			adjustedSize = (memorySizeInBytes / 1024.0 / 1024.0);
		}
		else
		{
			memUnitStr = (abbreviated ? "GB" : "Gigabytes");
			adjustedSize = (memorySizeInBytes / 1024.0 / 1024.0 / 1024.0);
		}

		// We only care about the first 2 decimal digits.
		std::snprintf(numStrBuf, sizeof(numStrBuf), "%.2f", adjustedSize);

		// Remove trailing zeros if no significant decimal digits:
		return removeTrailingFloatZeros(numStrBuf) + std::string(" ") + memUnitStr;
	}
}
