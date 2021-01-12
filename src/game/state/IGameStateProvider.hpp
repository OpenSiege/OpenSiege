
#pragma once

#include <string>

namespace ehb
{
    class IGameState;
    class IGameStateMgr;
    class IGameStateProvider
    {
    public:

        virtual ~IGameStateProvider() = default;

        virtual IGameState * createGameState(const std::string & gameStateType, IGameStateMgr & gameStateMgr) = 0;
    };
}
