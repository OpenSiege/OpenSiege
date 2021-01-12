
#pragma once

#include <string>

#include <osg/Group>

namespace ehb
{
    class ConsoleSignatureScanner
    {
    public:

        ConsoleSignatureScanner(const std::string& text);

        bool accept(const std::string& text);

        bool expect(const std::string& text);

        bool token(std::string::size_type& first_, std::string::size_type& last_);

    private:

        bool next();

    private:

        std::string data;
        std::string::size_type first, last;
    };
}