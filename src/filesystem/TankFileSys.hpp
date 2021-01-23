
#pragma once

#include "IFileSys.hpp"
#include "filesystem/TankFile.hpp"

#include <spdlog/spdlog.h>

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

        std::vector<std::unique_ptr<TankEntry>> eachTank;
        FileList cache;

        std::shared_ptr<spdlog::logger> log;
    };
}
