
#pragma once

#include <whereami.h>
#include <string>

namespace ehb
{
    class Platform final
    {
    public:

        Platform(Platform&&) = delete;
        Platform(const Platform&) = delete;

        Platform& operator = (Platform&&) = delete;
        Platform& operator = (const Platform&) = delete;       

        static Platform& instance();

        const std::string& getExecutablePath() const;

    private:

        Platform();

    private:

        std::string mExePath;
    };
}