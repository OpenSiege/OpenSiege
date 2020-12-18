
#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include "IFileSys.hpp"

namespace fs = std::filesystem;

namespace ehb
{
    // TODO: remove this class when we swap to the tank file system which can include functionality to read from the local file system
    class LocalFileSys : public IFileSys
    {
    public:

        LocalFileSys() = default;

        virtual ~LocalFileSys() = default;

        virtual bool init(IConfig& config) override;

        virtual InputStream createInputStream(const std::string & filename) override;

        virtual FileList getFiles() const override;
        virtual FileList getDirectoryContents(const std::string & directory) const override;

    private:

        //! the filesys currently requires the --ds-install-path to be set directly to the games fully extracted bits
        fs::path installPath;

        //! optional bits directory for mods
        fs::path bitsPath;

        std::shared_ptr<spdlog::logger> log;
    };
}
