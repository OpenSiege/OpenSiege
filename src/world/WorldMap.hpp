
#pragma once

#include "IFileSys.hpp"

#include <unordered_map>
#include <optional>

#include <osg/MatrixTransform>

#include <spdlog/spdlog.h>

namespace osg
{
    class Group;
}

namespace ehb
{
    class IFileSys;
    class WorldMap
    {
    public:

        WorldMap();
        ~WorldMap() = default;

        bool init(IFileSys& fileSys, const std::string& worldName, const std::string& regionName);

        void set(osg::Group& root);

        void update(double deltaTime);

        //! TEMP to 
        osg::MatrixTransform *activeRegion;

    private:

        //! keeps track of what proxys are currently being processed
        std::unordered_map<uint32_t, osg::MatrixTransform *> loadingQueue;

        //! holds the transform that was loaded in via loadingQueue
        std::unordered_map<uint32_t, osg::ref_ptr<osg::MatrixTransform>> eachRegion;

        //! holds the transforms of all the nodes currently loaded into the WorldMap
        std::unordered_map<uint32_t, osg::MatrixTransform*> eachNode;

        //! the guid of the region that is queued for loading
        std::optional<uint32_t> guid;

        std::shared_ptr<spdlog::logger> log;
    };

    inline WorldMap::WorldMap()
    {
        log = spdlog::get("log");
    }
}
