
#pragma once

#include "state/IGameState.hpp"

namespace ehb
{
    class IConfig;
    class IFileSys;
    class GasTestState final : public IGameState
    {
    public:

        GasTestState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys& fileSys);

        virtual ~GasTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;
    };

    inline GasTestState::GasTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys)
    {
    }
}
