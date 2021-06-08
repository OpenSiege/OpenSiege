
#pragma once

#include "IGameState.hpp"

namespace osg
{
    class Group;
}

namespace ehb
{
    class IConfig;
    class IFileSys;
    class ContentDb;
    class Console;
    class Shell;
    class InitState final : public IGameState
    {
    public:

        //! I'm not a huge fan of passing through the pointers for the 2 osg groups. these need to be elevated to a subsystem somewhere
        InitState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys & fileSys, ContentDb& contentDb, Console& console, Shell& shell, osg::Group* scene3d, osg::Group* scene2d);

        virtual ~InitState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;
        ContentDb& contentDb;
        Console& console;
        Shell& shell;

        // seems hacky
        osg::Group* scene3d = nullptr, *scene2d = nullptr;
    };

    inline InitState::InitState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, Console& console, Shell& shell, osg::Group* scene3d, osg::Group* scene2d) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), contentDb(contentDb), console(console), shell(shell), scene3d(scene3d), scene2d(scene2d)
    {
    }
}
