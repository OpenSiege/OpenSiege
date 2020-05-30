
#pragma once

#include "IGameState.hpp"

namespace ehb
{
    class IConfig;
    class TestState : public IGameState
    {
    public:

        TestState(IGameStateMgr & gameStateMgr, IConfig & config);

        virtual ~TestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
    };

    inline TestState::TestState(IGameStateMgr& gameStateMgr, IConfig& config) : gameStateMgr(gameStateMgr), config(config)
    {
    }
}
