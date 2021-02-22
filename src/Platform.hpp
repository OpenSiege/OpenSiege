
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

    public:

        struct Git
        {
            static const std::string GIT_SHA1;
            static const std::string GIT_BRANCH;
        };

    private:

        Platform();

    private:

        std::string mExePath;
    };
}