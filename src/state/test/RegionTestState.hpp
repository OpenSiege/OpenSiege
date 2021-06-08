
#pragma once

#include "state/IGameState.hpp"

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
    class Region;
    class SiegeNodeMesh;
    class RegionTestState final : public IGameState
    {
    public:

        RegionTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene);

        virtual ~RegionTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr& gameStateMgr;
        IConfig& config;
        IFileSys& fileSys;
        ContentDb& contentDb;

        osgViewer::Viewer& viewer;
        osg::Group& scene;

        Region* region = nullptr;
        SiegeNodeMesh* selectedSiegeNode = nullptr;
        
        std::shared_ptr<spdlog::logger> log;
    };

    inline RegionTestState::RegionTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), contentDb(contentDb), scene(scene), viewer(viewer)
    {
    }
}
