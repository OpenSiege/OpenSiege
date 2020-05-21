
#include "Game.hpp"

#include "cfg/IConfig.hpp"
#include "state/InitState.hpp"

namespace ehb
{
    Game::Game(IConfig & config) : config(config), gameStateMgr(this)
    {
    }

    int Game::exec()
    {
        if (config.getBool("fullscreen"))
        {
        }
        else
        {
            int defaultX = 0, defaultY = 0;

            int actualWidth = config.getInt("width", 800);
            int actualHeight = config.getInt("height", 600);
        }

        gameStateMgr.request("InitState");

        // TODO: write out config values which belong in the ini file

        return 0;
    }


    IGameState* Game::createGameState(const std::string& gameStateType, IGameStateMgr& gameStateMgr)
    {
        if (gameStateType == "InitState")
        {
            return new InitState(gameStateMgr, config);
        }

        return nullptr;
    }

    bool Game::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action, osg::Object*, osg::NodeVisitor*)
    {
        return gameStateMgr.handle(event, action);
    }

}
