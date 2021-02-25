
#pragma once

#include <optional>
#include <filesystem>

#include "IFileSys.hpp"
#include "filesystem/TankFile.hpp"

#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace ehb
{
    struct TankEntry
    {
        TankFile tank;
        TankFile::Reader reader;
    };

    class IConfig;
    class TankFileSys : public IFileSys
    {
    public:

        virtual ~TankFileSys() = default;

        virtual bool init(IConfig& config) override;

        virtual InputStream createInputStream(const std::string & filename) override;

        virtual FileList getFiles() const override;
        virtual FileList getDirectoryContents(const std::string & directory) const override;

    private:

        //! store tank files, this vector removed duplicates and orders by priority
        std::vector<std::unique_ptr<TankEntry>> eachTank;

        //! contains a full list of files from the tanks that are loaded
        FileList cache;

        //! the optional bits path
        std::optional<fs::path> bits;

        std::shared_ptr<spdlog::logger> log;
    };
}
