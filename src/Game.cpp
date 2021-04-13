
#include "Game.hpp"

#include <minIni.h>
#include <osg/MatrixTransform>
#include <osg/ClipControl>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include "Platform.hpp"
#include "cfg/IConfig.hpp"
#include "state/InitState.hpp"
#include "state/ExitState.hpp"
#include "state/test/GasTestState.hpp"
#include "state/test/SiegeNodeTestState.hpp"
#include "state/test/UITestState.hpp"
#include "state/test/RegionTestState.hpp"
#include "state/test/AspectMeshTestState.hpp"
#include "state/test/TankTestState.hpp"

namespace ehb
{
    Game::Game(IConfig & config) : config(config), gameStateMgr(this), scene3d(new osg::Group), scene2d(new osg::Group), console(new Console(config, fileSys, gameStateMgr, *scene3d, *scene2d))
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

            int actualWidth = config.getInt("width", 640);
            int actualHeight = config.getInt("height", 480);

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
        traits->windowName = "Open Siege - " + Platform::Git::GIT_BRANCH + " - " + Platform::Git::GIT_SHA1;
        // traits->useCursor = false;
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
        osg::ref_ptr<osg::Camera> cam = new osg::Camera(*viewer.getCamera());
        cam->setGraphicsContext(gc);
        viewer.setCamera(cam);

        viewer.setReleaseContextAtEndOfFrameHint(false);
        viewer.realize();
        viewer.init();

        viewer.setLightingMode(osg::View::NO_LIGHT);

        { // hook into the osg event system
            viewer.addEventHandler(this);

            viewer.addEventHandler(new osgViewer::StatsHandler);
        }

        setupScene();

        // TODO: move this when we get our camera manipulator implemented
        viewer.setCameraManipulator(new osgGA::TrackballManipulator());

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

        const fs::path iniFileName = fs::path(config.getString("config-dir", ".")) / "OpenSiege.ini";
        minIni ini(iniFileName.string());

        if (!config.getString("bits", "").empty()) ini.put("OpenSiege", "bits", config.getString("bits", ""));

        spdlog::get("filesystem")->info("Finished writing INI to {}", iniFileName.string());

        return 0;
    }


    IGameState* Game::createGameState(const std::string& gameStateType, IGameStateMgr& gameStateMgr)
    {
        if (gameStateType == "InitState")
        {
            return new InitState(gameStateMgr, config, fileSys, contentDb, *console);
        }
        else if (gameStateType == "ExitState")
        {
            return new ExitState(gameStateMgr, config, viewer);
        }
        else if (gameStateType == "GasTestState")
        {
            return new GasTestState(gameStateMgr, config, fileSys);
        }
        else if (gameStateType == "SiegeNodeTestState")
        {
            return new SiegeNodeTestState(gameStateMgr, config, fileSys, viewer, *scene3d);
        }
        else if (gameStateType == "UITestState")
        {
            return new UITestState(gameStateMgr, viewer, fileSys, *scene2d);
        }
        else if (gameStateType == "RegionTestState")
        {
            return new RegionTestState(gameStateMgr, config, fileSys, contentDb, viewer, *scene3d);
        }
        else if (gameStateType == "AspectMeshTestState")
        {
            return new AspectMeshTestState(gameStateMgr, config, fileSys, viewer, contentDb, *scene3d);
        }
        else if (gameStateType == "TankTestState")
        {
            return new TankTestState(gameStateMgr, config, fileSys);
        }

        return nullptr;
    }

    bool Game::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action, osg::Object*, osg::NodeVisitor*)
    {
        if (event.getEventType() == osgGA::GUIEventAdapter::RESIZE)
        {
            // resize cameras, etc...
            if (scene2d && scene2d->getNumParents() != 0)
            {
                if (auto camera = scene2d->getParent(0)->asCamera())
                {
                    camera->setProjectionMatrix(osg::Matrix::ortho2D(0, event.getWindowWidth(), 0, event.getWindowHeight()));
                }
            }
        }

        // this feels a bit hacky
        if (event.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            if (event.getKey() == osgGA::GUIEventAdapter::KEY_Backquote)
            {
                console->toggle();

                return true;
            }
        }

        // this essentially causes the console to be modal when it is displayed
        if (console->active())
        {
            return console->handle(event, action);
        }

        return gameStateMgr.handle(event, action);
    }

    void Game::setupScene()
    {
        // create the absolute root of our graph - don't manually attach anything to this
        osg::Group* root = new osg::Group;

        // this should never need to touched
        viewer.setSceneData(root);

        if (auto rotation = new osg::MatrixTransform)
        {
            // create our 90 degree rotation, im still not totally sure this is correct
            rotation->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(90.f), osg::Vec3(1, 0, 0)));

            // create the 3d graph - this is where all 3d elements should be attached
            rotation->addChild(scene3d);

            // our rotation should be directly under the root and is a sibling to the gui camera
            root->addChild(rotation);
        }

        if (auto camera = new osg::Camera)
        {
            // create our 2d camera and attach the gui scene
            camera->setProjectionMatrix(osg::Matrix::ortho2D(0, config.getInt("width", 640), 0, config.getInt("height", 480)));
            camera->setViewport(0, 0, config.getInt("width", 640), config.getInt("height", 480));
            camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
            camera->setViewMatrix(osg::Matrix::identity());
            camera->setClearMask(GL_DEPTH_BUFFER_BIT);
            camera->setRenderOrder(osg::Camera::POST_RENDER);
            camera->setAllowEventFocus(false);
            camera->addChild(scene2d);

            // turn off lighting for the gui camera and small screen culling
            camera->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
            camera->getOrCreateStateSet()->setAttribute(new osg::ClipControl(osg::ClipControl::UPPER_LEFT), osg::StateAttribute::OVERRIDE);
            camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

            // attach the 2d camera to the graph
            root->addChild(camera);

            // the console is special as its not managed by the shell, it is essentially global
            camera->addChild(console);
        }
    }
}
