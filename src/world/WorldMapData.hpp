
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
            WorldMapData(IFileSys& fileSys, const std::string& path);

            struct StitchData
            {
                uint32_t node1, node2;
                int32_t door1, door2;
            };

            std::string name;
            std::string description;
            std::string screen_name;

            //! store the region name and the guid of that region
            std::unordered_map<std::string, uint32_t> nameMap;

            //! store the nodeguid against which region it belongs to
            std::unordered_map<uint32_t, uint32_t> nodeMap;

            std::unordered_map<uint32_t, std::unordered_map<uint32_t, StitchData>> stitchData;
        };

    public:

        WorldMapDataCache() = default;

        void init(IFileSys& fileSys);

    private:

        //! hardcoded path to DS map files
        inline static const std::string worldMapsPath = "/world/maps/";

        std::shared_ptr<spdlog::logger> log;

        std::unordered_map<std::string, WorldMapData> cache;
    };
}