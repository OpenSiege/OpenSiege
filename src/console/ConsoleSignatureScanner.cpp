
#include "ConsoleSignatureScanner.hpp"

namespace ehb
{
    ConsoleSignatureScanner::ConsoleSignatureScanner(const std::string& text) : first(0), last(0)
    {
        data.resize(text.size());

        for (std::string::size_type i = 0; i < text.size(); i++)
        {
            data[i] = std::tolower(text[i]);
        }

        if (!next()) throw "no token found";
    }

    bool ConsoleSignatureScanner::accept(const std::string& text)
    {
        if (data.compare(first, last - first, text) == 0)
        {
            next();
            return true;
        }

        return false;
    }

    bool ConsoleSignatureScanner::expect(const std::string& text)
    {
        if (!accept(text))
        {
            return false;
            // throw "unexpected token found";
        }
    }

    bool ConsoleSignatureScanner::token(std::string::size_type& first_, std::string::size_type& last_)
    {
        if (last < data.size())
        {
            first_ = first;
            last_ = last;

            next();

            return true;
        }

        return false;
    }

    bool ConsoleSignatureScanner::next()
    {
        if (last < data.size())
        {
            first = data.find_first_not_of(' ', last);

            if (first == std::string::npos) { last = first; }

            if (data[first] == '\n') { last = first + 1; }
            else { last = data.find_first_of(" \n", first); }

            // std::cout << "found token: '" << data.substr(first, last - first) << "' @ (" << first << " to " << last << ") -> " << data[first] << std::endl;

            return true;
        }

        return false;
    }
}