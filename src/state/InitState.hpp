
#pragma once

#include "IGameState.hpp"

namespace ehb
{
    class IConfig;
    class IFileSys;
    class InitState : public IGameState
    {
    public:

        InitState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys & fileSys);

        virtual ~InitState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;
    };

    inline InitState::InitState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys)
    {
    }
}
