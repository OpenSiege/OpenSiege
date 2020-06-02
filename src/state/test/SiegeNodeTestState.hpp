
#pragma once

#include "state/IGameState.hpp"

namespace ehb
{
    class IConfig;
    class IFileSys;
    class SiegeNodeTestState : public IGameState
    {
    public:

        SiegeNodeTestState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys& fileSys, osg::Group& scene);

        virtual ~SiegeNodeTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;

        osg::Group& scene;
    };

    inline SiegeNodeTestState::SiegeNodeTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), scene(scene)
    {
    }
}
