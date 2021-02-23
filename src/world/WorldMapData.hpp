
#pragma once

#include <unordered_map>
#include <spdlog/spdlog.h>

#include "SiegePos.hpp"

namespace ehb
{
    class IFileSys;
    class WorldMapDataCache
    {
    public:

        struct WorldMapData
        {
        };

    public:

        WorldMapDataCache() = default;

        void init(IFileSys& fileSys);

    private:

        std::shared_ptr<spdlog::logger> log;

        std::unordered_map<std::string, WorldMapData> cache;
    };
}