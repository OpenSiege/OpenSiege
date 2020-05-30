
#pragma once

#include "IGameState.hpp"

namespace ehb
{
    class IConfig;
    class InitState : public IGameState
    {
    public:

        InitState(IGameStateMgr & gameStateMgr, IConfig & config);

        virtual ~InitState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
    };

    inline InitState::InitState(IGameStateMgr& gameStateMgr, IConfig& config) : gameStateMgr(gameStateMgr), config(config)
    {
    }
}
