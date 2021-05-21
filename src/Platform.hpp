
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

        // not a huge fan of this
        void generateCapabilities();

        const std::string& getExecutablePath() const;

    public:

        struct Git
        {
            static const std::string GIT_SHA1;
            static const std::string GIT_BRANCH;
        };

        //! make sure the below are set to minimum spec defaults
        struct Capabilities
        {
            std::string vendor;
            std::string renderer;
            std::string version;

            int32_t numProcessors = 1;
            
            float GLSLversion = 1.0f;

            bool supportsGLSL = false;

        } capabilities;

    private:

        Platform();

    private:

        std::string mExePath;
    };
}