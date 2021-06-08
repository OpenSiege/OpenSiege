
#pragma once

#include "state/IGameState.hpp"

//! The below state is used for rapid copy and paste prototyping - don't call it

namespace ehb
{
    class IConfig;
    class TestState final : public IGameState
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
