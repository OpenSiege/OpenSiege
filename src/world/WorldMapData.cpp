
#include "WorldMapData.hpp"

namespace ehb
{
    void WorldMapDataCache::init(IFileSys& fileSys)
    {
        log = spdlog::get("world");

        log->info("Caching WorldMapData from filesystem");
    }
}