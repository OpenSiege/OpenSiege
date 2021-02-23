
#include "WorldMapData.hpp"

#include "IFileSys.hpp"

namespace ehb
{
    WorldMapDataCache::WorldMapData::WorldMapData(IFileSys& fileSys, const std::string& path)
    {
        auto log = spdlog::get("world");

        const std::string stitchIndexDotGas = path + "/index/stitch_index.gas";
        const std::string mainDotGas = path + "/main.gas";
        const std::string regionsPath = path + "/regions";
        const std::string nodesDotGas = path + "/terrain_nodes/nodes.gas";

        std::unordered_map<uint32_t, uint32_t> node2region;

        if (auto stream = fileSys.createInputStream(stitchIndexDotGas))
        {
            if (Fuel doc; doc.load(*stream))
            {
                log->info("Reading stitch index @ {}", stitchIndexDotGas);

                const auto root = doc.child("stitch_index");

                for (const auto node : root->eachChild())
                {
                    const uint32_t regionGuid = std::stoul(node->name(), nullptr, 16);

                    for (const auto attrib : node->eachAttribute())
                    {
                        if (const auto itr = attrib.value.find(','); itr != std::string::npos)
                        {
                            const uint32_t nodeGuid = std::stoul(attrib.value.substr(0, itr), nullptr, 16);

                            node2region.emplace(nodeGuid, regionGuid);
                        }
                    }
                }

                for (auto node : root->eachChild())
                {
                    if (node->type() == "stitch_index")
                    {
                        const uint32_t regionGuid = std::stoul(node->name(), nullptr, 16);

                        for (const auto entry : node->eachAttribute())
                        {
                            const auto i1 = entry.value.find(',', 0);
                            const auto i2 = entry.value.find(',', i1 + 1);
                            const auto i3 = entry.value.find(',', i2 + 1);

                            const uint32_t node1 = std::stoul(entry.value.substr(0, i1), nullptr, 16);
                            const int32_t door1 = std::stoi(entry.value.substr(i1 + 1, i2 - i1 - 1));
                            const uint32_t node2 = std::stoul(entry.value.substr(i2 + 1, i3 - i2 - 1), nullptr, 16);
                            const int32_t door2 = std::stoi(entry.value.substr(i3 + 1, std::string::npos));

                            if (const auto itr = node2region.find(node2); itr != node2region.end())
                            {
                                const uint32_t region1 = regionGuid;
                                const uint32_t region2 = itr->second;

                                stitchData[region1].emplace(region2, StitchData{ node1, node2, door1, door2 });
                            }
                            else
                            {
                                log->error("CRITICAL: invalid stitch map");
                            }
                        }
                    }
                }
            }
        }
        else
        {
            log->error("CRITICAL: No stitch index found @ {}", stitchIndexDotGas);
        }

        if (auto stream = fileSys.createInputStream(mainDotGas))
        {
            if (Fuel doc; doc.load(*stream))
            {
                log->info("Reading main.gas @ {}", mainDotGas);

                name = doc.child("map")->valueOf("name");
                description = doc.child("map")->valueOf("description");
                screen_name = doc.child("map")->valueOf("screen_name");

                log->info("World Info - name [{}] description [{}] screen_name [{}]", name, description, screen_name);
            }
        }

        for (auto region : fileSys.getDirectoryContents(regionsPath))
        {
            uint32_t regionGuid = 0;

            if (auto stream = fileSys.createInputStream(region + "/main.gas"))
            {
                if (Fuel doc; doc.load(*stream))
                {
                    log->info("Reading {}", region + "/main.gas");

                    regionGuid = doc.child("region")->valueAsUInt("guid");
                    nameMap.emplace(region + "/terrain_nodes/nodes.gas", regionGuid);
                }
            }

            if (auto stream = fileSys.createInputStream(region + "/terrain_nodes/nodes.gas"))
            {
                std::string line;
                while (std::getline(*stream, line))
                {
                    if (line.compare(1, 11, "[t:snode,n:") == 0)
                    {
                        const uint32_t nodeGuid = std::stoul(line.substr(12, 10), nullptr, 16);

                        nodeMap.emplace(nodeGuid, regionGuid);
                    }
                }
            }
        }
    }

    void WorldMapDataCache::init(IFileSys& fileSys)
    {
        log = spdlog::get("world");

        log->info("Staring caching of WorldMapData from {}", worldMapsPath);

        for (auto path : fileSys.getDirectoryContents(worldMapsPath))
        {
            cache.emplace(fileSys.filename(path), WorldMapData(fileSys, path));

            log->info("World [{}] @ {} has been cached", fileSys.filename(path), path);
        }
    }
}