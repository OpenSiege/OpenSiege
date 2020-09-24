
#pragma once

#include "state/IGameState.hpp"

#include <unordered_map>

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class IConfig;
    class IFileSys;
    class RegionTestState : public IGameState
    {
    public:

        RegionTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, osgViewer::Viewer& viewer, osg::Group& scene);

        virtual ~RegionTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr& gameStateMgr;
        IConfig& config;
        IFileSys& fileSys;

        osgViewer::Viewer& viewer;
        osg::Group& scene;

        uint32_t targetGuid = 0;

    private:

        const std::string& resolveFileName(const std::string& filename) const;
    };

    inline RegionTestState::RegionTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, osgViewer::Viewer& viewer, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), scene(scene), viewer(viewer)
    {
    }
}
