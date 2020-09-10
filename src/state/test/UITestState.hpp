
#pragma once

#include "state/IGameState.hpp"

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class IConfig;
    class IFileSys;
    class UITestState : public IGameState
    {
    public:

        UITestState(IGameStateMgr & gameStateMgr, osgViewer::Viewer& viewer, IFileSys& fileSys, osg::Group& scene);

        virtual ~UITestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IFileSys& fileSys;
        osgViewer::Viewer& viewer;

        osg::Group& scene;
    };

    inline UITestState::UITestState(IGameStateMgr& gameStateMgr, osgViewer::Viewer &viewer, IFileSys& fileSys, osg::Group& scene) : gameStateMgr(gameStateMgr), viewer(viewer), fileSys(fileSys), scene(scene)
    {
    }
}
