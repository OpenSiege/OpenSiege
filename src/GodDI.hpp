
#pragma once

namespace osg
{
    class Group;
}

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class IConfig;
    class GameStateMgr;
    class TankFileSys;
    class ContentDb;
    class Shell;

    // Temporary object to pass for DI to make my life easier
    class GodDI final
    {
    public:

        explicit GodDI(IConfig& config, GameStateMgr& gameStateMgr, TankFileSys& tankFileSys, ContentDb& contentDb, osg::Group& scene3d, osg::Group& scene2d);

        IConfig& config;
        GameStateMgr& gameStateMgr;
        TankFileSys& tankFileSys;
        ContentDb& contentDb;

        osg::Group& scene3d;
        osg::Group& scene2d;
    };

    inline GodDI::GodDI(IConfig& config, GameStateMgr& gameStateMgr, TankFileSys& tankFileSys, ContentDb& contentDb, osg::Group& scene3d, osg::Group& scene2d) :
        config(config),
        gameStateMgr(gameStateMgr),
        tankFileSys(tankFileSys),
        contentDb(contentDb),
        scene3d(scene3d),
        scene2d(scene2d)
    {

    }
}