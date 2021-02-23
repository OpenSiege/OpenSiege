
#pragma once

#include "state/IGameState.hpp"

#include <optional>
#include <spdlog/spdlog.h>

namespace osg
{
    class MatrixTransform;
}

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class IConfig;
    class IFileSys;
    class ContentDb;
    class WorldMapDataCache;
    class FullMapTestState : public IGameState
    {
    public:

        FullMapTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene, WorldMapDataCache& worldMapData);

        virtual ~FullMapTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr& gameStateMgr;
        IConfig& config;
        IFileSys& fileSys;
        ContentDb& contentDb;
        WorldMapDataCache& worldMapDataCache;

        osgViewer::Viewer& viewer;
        osg::Group& scene;
        
        //! the guid of the first region that will be loaded
        std::optional<uint32_t> guid;

        //! each region guid that has/is being processed
        std::set<uint32_t> eachGuid;

        // loadingQueue keeps track of what proxys are currently being processed
        std::unordered_map<uint32_t, osg::MatrixTransform*> loadingQueue;

        // eachRegion holds the the transform that was loaded in via loadingQueue
        // eachNode holds the transforms of all the nodes currently loaded into the world
        std::unordered_map<uint32_t, osg::MatrixTransform*> eachRegion, eachNode;

        std::shared_ptr<spdlog::logger> log;
    };

    inline FullMapTestState::FullMapTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene, WorldMapDataCache& worldMapDataCache) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), contentDb(contentDb), scene(scene), viewer(viewer), worldMapDataCache(worldMapDataCache)
    {
    }
}
