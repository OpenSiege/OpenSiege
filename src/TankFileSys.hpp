
#pragma once

#include "IFileSys.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    class IConfig;
    class TankFileSys : public IFileSys
    {
    public:

        virtual ~TankFileSys() = default;

        virtual InputStream createInputStream(const std::string & filename) override;

        virtual FileList getFiles() const override;
        virtual FileList getDirectoryContents(const std::string & directory) const override;

        void init(IConfig & config);

    private:

        std::shared_ptr<spdlog::logger> log;
    };
}
