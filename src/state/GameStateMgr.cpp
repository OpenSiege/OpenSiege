
#include "GameStateMgr.hpp"

#include <osgGA/GUIEventAdapter>
#include <spdlog/spdlog.h>
#include "IGameState.hpp"
#include "IGameStateProvider.hpp"

namespace ehb
{
    GameStateMgr::GameStateMgr(IGameStateProvider * provider) : provider(provider)
    {
    }

    GameStateMgr::~GameStateMgr()
    {
        if (currState.second)
        {
            currState.second->leave();
        }
    }

    void GameStateMgr::request(const std::string & gameStateType)
    {
        if (IGameState * state = provider->createGameState(gameStateType, *this))
        {
            if (pendState.second)
            {
                spdlog::get("log")->warn("GameStateMgr still has a pending state {} while requesting a new state ({})", pendState.first, gameStateType);
            }

            pendState.first = gameStateType;
            pendState.second.reset(state);
        }
        else
        {
            spdlog::get("log")->warn("GameState [{}] is unavailable", gameStateType);
        }
    }

    void GameStateMgr::update(double deltaTime)
    {
        if (pendState.second)
        {
            auto log = spdlog::get("log");

            if (currState.second)
            {
                log->info("==========================================================================================================================");
                log->info("| BEGIN - WORLD STATE TRANSITION: {} --> {}", currState.first, pendState.first);
                log->info("|");

                currState.second->leave();
            }
            else
            {
                log->info("==========================================================================================================================");
                log->info("| BEGIN - WORLD STATE TRANSITION: {}", pendState.first);
                log->info("|");
            }

            prevState.first = currState.first;
            prevState.second = std::move(currState.second);

            currState.first = pendState.first;
            currState.second = std::move(pendState.second);

            currState.second->enter();

            log->info("|");
            log->info("| END - WORLD STATE TRANSITION. State = {}", pendState.first);
            log->info("==========================================================================================================================");
        }

        if (currState.second)
        {
            currState.second->update(deltaTime);
        }
    }

    bool GameStateMgr::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        // assert(currentState != nullptr);

        switch (event.getEventType())
        {
            // TODO: when application is shutting down then call currentState->leave(); currentState.reset();

            case osgGA::GUIEventAdapter::USER:
                break;

            default:
                break;
        }

        return currState.second->handle(event, action);
    }

    std::string GameStateMgr::currentStateName() const
    {
        return currState.first;
    }

    std::string GameStateMgr::previousStateName() const
    {
        return prevState.first;
    }

    std::string GameStateMgr::pendingStateName() const
    {
        return pendState.first;
    }
}
