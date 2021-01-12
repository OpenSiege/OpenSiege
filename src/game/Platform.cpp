
#include "Platform.hpp"

namespace ehb
{
    Platform::Platform()
    {
        const int length = wai_getExecutablePath(nullptr, 0, nullptr);
        std::string exe(length, '\0');
        wai_getExecutablePath(exe.data(), length, nullptr);

        mExePath = std::move(exe);
    }

    Platform &Platform::instance()
    {
        static Platform platform;
        return platform;
    }

    const std::string& Platform::getExecutablePath() const
    {
        return mExePath;
    }
}