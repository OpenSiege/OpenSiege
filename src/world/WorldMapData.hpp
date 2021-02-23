
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

            const std::unordered_map<uint32_t, StitchData>& eachStitchForRegion(uint32_t regionGuid) const
            {
                static const std::unordered_map<uint32_t, StitchData> empty;

                const auto itr = stitchData.find(regionGuid);
                return itr != stitchData.end() ? itr->second : empty;
            }

            std::string nameForRegionGuid(uint32_t guid) const
            {
                for (const auto& entry : nameMap)
                {
                    if (entry.second == guid)
                    {
                        return entry.first;
                    }
                }

                return {};
            }

            //! pass a node guid to get its owning region guid
            uint32_t regionForNode(uint32_t nodeGuid) const
            {
                if (const auto itr = nodeMap.find(nodeGuid); itr != nodeMap.end())
                {
                    return itr->second;
                }

                return 0;
            }

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

        std::unordered_map<std::string, WorldMapData> cache;

    private:

        //! hardcoded path to DS map files
        inline static const std::string worldMapsPath = "/world/maps/";

        std::shared_ptr<spdlog::logger> log;
    };
}