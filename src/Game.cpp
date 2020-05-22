
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
            viewer.setUpViewOnSingleScreen();
        }
        else
        {
            int defaultX = 0, defaultY = 0;

            int actualWidth = config.getInt("width", 800);
            int actualHeight = config.getInt("height", 600);

            // calculate the coordinates which would put the window at the center of the screen
            if (osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface())
            {
                osg::GraphicsContext::ScreenSettings resolution;

                // TODO: consider not hard coding to a specific monitor
                wsi->getScreenSettings(0, resolution);

                defaultX = (resolution.width - actualWidth) / 2;
                defaultY = (resolution.height - actualHeight) / 2;
            }

            viewer.setUpViewInWindow(config.getInt("x", defaultX), config.getInt("y", defaultY), actualWidth, actualHeight);
        }

        // set the title and disable the mouse cursor for the window
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(*viewer.getCamera()->getGraphicsContext()->getTraits());
        traits->windowName = "Open Siege - Dev";
        traits->useCursor = false;
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
        osg::ref_ptr<osg::Camera> cam = new osg::Camera(*viewer.getCamera());
        cam->setGraphicsContext(gc);
        viewer.setCamera(cam);

        viewer.setReleaseContextAtEndOfFrameHint(false);
        viewer.realize();
        viewer.init();

        { // hook into the osg event system
            viewer.addEventHandler(this);
        }

        gameStateMgr.request("InitState");

        osg::Timer fps;

        const float maxfps = static_cast<float>(config.getInt("maxfps", -1));

        while (!viewer.done())
        {
            const double deltaTime = fps.time_s();

            fps.setStartTick();

            // update all systems that require an update
            gameStateMgr.update(deltaTime);

            // obey maxfps
            if (maxfps > 0.f)
            {
                const double thisFrameTime = deltaTime;
                const double minFrameTime = 1.0 / maxfps;

                if (thisFrameTime < minFrameTime)
                {
                    OpenThreads::Thread::microSleep(1000 * 1000 * (minFrameTime - thisFrameTime));
                }
            }

            viewer.advance();

            viewer.eventTraversal();
            viewer.updateTraversal();
            viewer.renderingTraversals();
        }

        { // cleanup osg hooks
            viewer.removeEventHandler(this);
        }

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
