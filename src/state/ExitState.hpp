
#pragma once

#include "IGameState.hpp"

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class IConfig;
    class ExitState : public IGameState
    {
    public:

        ExitState(IGameStateMgr & gameStateMgr, IConfig & config, osgViewer::Viewer& viewer);

        virtual ~ExitState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;

        osgViewer::Viewer& viewer;
    };

    inline ExitState::ExitState(IGameStateMgr& gameStateMgr, IConfig& config, osgViewer::Viewer& viewer) : gameStateMgr(gameStateMgr), config(config), viewer(viewer)
    {
    }
}
