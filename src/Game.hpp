
#pragma once

#include "state/IGameStateProvider.hpp"
#include "state/GameStateMgr.hpp"
#include "filesystem/LocalFileSys.hpp"
#include "filesystem/TankFileSys.hpp"
#include "ContentDb.hpp"

#include "console/Console.hpp"

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

namespace ehb
{
    // TODO: do not inherit from osgGA::GUIEventHandler but instead create a separate class just for this
    class IConfig;
    class Game final : public IGameStateProvider, public osgGA::GUIEventHandler
    {
    public:

        Game(IConfig & config);

        int exec();

    private:

        virtual ~Game() = default;

        virtual IGameState* createGameState(const std::string& gameStateType, IGameStateMgr& gameStateMgr) override;

        virtual bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action, osg::Object*, osg::NodeVisitor*) override;

    private:

        void setupScene();

        IConfig& config;
        GameStateMgr gameStateMgr;
        // LocalFileSys fileSys; // TODO: remove local file system in favor of tank file system
        TankFileSys fileSys;
        ContentDb contentDb;

        osgViewer::Viewer viewer;

        osg::ref_ptr<osg::Group> scene3d;
        osg::ref_ptr<osg::Group> scene2d;

        // console needs to be last to ensure all systems are booted
        osg::ref_ptr<Console> console;
    };
}
