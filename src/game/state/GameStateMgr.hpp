
#pragma once

#include <memory>
#include <string>
#include "IGameState.hpp"
#include "IGameStateMgr.hpp"

namespace osgGA
{
    class GUIActionAdapter;
    class GUIEventAdapter;
}

namespace ehb
{
    class IGameStateProvider;
    class GameStateMgr final : public IGameStateMgr
    {
    public:

        GameStateMgr(IGameStateProvider * provider);

        virtual ~GameStateMgr();

        virtual void request(const std::string & gameStateType) override;

        void update(double deltaTime);
        bool handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action);

        virtual std::string currentStateName() const override;
        virtual std::string previousStateName() const override;
        virtual std::string pendingStateName() const override;

    private:

        IGameStateProvider * provider;

        std::pair<std::string, std::unique_ptr<IGameState>> currState;
        std::pair<std::string, std::unique_ptr<IGameState>> pendState;
        std::pair<std::string, std::unique_ptr<IGameState>> prevState;
    };
}
