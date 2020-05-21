
#include "Game.hpp"

#include "cfg/IConfig.hpp"

namespace ehb
{
    Game::Game(IConfig & config) : config(config)
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

        // TODO: write out config values which belong in the ini file

        return 0;
    }
}
