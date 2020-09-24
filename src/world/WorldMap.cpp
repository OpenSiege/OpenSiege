
#include "WorldMap.hpp"

#include "gas/Fuel.hpp"

#include <osgDB/ReadFile>

namespace ehb
{
    bool WorldMap::init(IFileSys& fileSys, const std::string& worldName, const std::string& regionName)
    {
        // check if we are loading a region
        if (guid) return false;

        static const std::string maps = "/world/maps/";
        static const std::string world = maps + worldName + "/";
        static const std::string mainDotGas = world + "main.gas";

        // an existent main.gas for the world is a requirement
        if (auto stream = fileSys.createInputStream(mainDotGas))
        {
        }
        else
        {
            log->error("fatal error trying to load the world {} as main.gas doesn't exist", worldName);

            return false;
        }

        static const std::string regions = world + "regions/";
        static const std::string regionPath = regions + regionName + "/";
        static const std::string regionMainDotGas = regionPath + "main.gas";
        static const std::string nodesDotGas = regionPath + "terrain_nodes/nodes.gas";

        uint32_t regionGuid = 0;
        if (auto stream = fileSys.createInputStream(regionMainDotGas))
        {
            if (Fuel doc; doc.load(*stream))
            {
                regionGuid = doc.valueAsUInt("region:guid");
            }
        }

        if (regionGuid == 0)
        {
            log->error ("there is a main.gas for region {} under world {} but the guid couldn't be resolved", regionName, worldName);
            
            return false;
        }

        osg::MatrixTransform* region = static_cast<osg::MatrixTransform*> (osgDB::readNodeFile(nodesDotGas));

        if (region != nullptr)
        {
            log->info("worldmap loaded region {}", regionName);

            eachRegion.emplace(regionGuid, region);
        }
        else
        {
            log->error("worldmap failed to load region {} for worldmap {}", regionName, worldName);

            return false;
        }

        return true;
    }

    void WorldMap::set(osg::Group& root)
    {
        activeRegion = eachRegion.begin()->second;

        // manually force the first element of the map
        root.addChild(activeRegion);
    }
}
