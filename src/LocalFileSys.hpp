
#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include "IFileSys.hpp"

namespace fs = std::filesystem;

namespace ehb
{
    class LocalFileSys : public IFileSys
    {
    public:

        LocalFileSys();

        virtual ~LocalFileSys() = default;

        virtual InputStream createInputStream(const std::string & filename) override;

        virtual FileList getFiles() const override;
        virtual FileList getDirectoryContents(const std::string & directory) const override;

        void setRootDir(const fs::path & directory);

    private:

        fs::path rootDir;

        std::shared_ptr<spdlog::logger> log;
    };

    inline LocalFileSys::LocalFileSys()
    {
        log = spdlog::get("filesystem");
    }

    inline void LocalFileSys::setRootDir(const fs::path & directory)
    {
        rootDir = directory;
    }
}
