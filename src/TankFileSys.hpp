
#pragma once

#include "IFileSys.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    // TODO: implement tanks
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

        std::shared_ptr<spdlog::logger> log;
    };
}
