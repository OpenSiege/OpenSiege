
#pragma once

#include "state/IGameState.hpp"

#include <osg/MatrixTransform>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>

#include <spdlog/spdlog.h>

namespace osg
{
    class MatrixTransform;
}

namespace osgViewer
{
    class Viewer;
}

namespace ehb
{
    class Aspect;
    class ContentDb;
}

namespace ehb
{
    struct Player
    {
        uint32_t networkId = -1; // assigned by the network
        uint32_t goid = 0; // local to their own machine

        const std::string tmplName = "farmgirl"; // we are all farmgirls for now

        Aspect* mesh = nullptr;

        bool isFidgeting;
        osg::ref_ptr<osgAnimation::Animation> fidgetAnimation = nullptr, walkAnimation = nullptr;
        osg::ref_ptr<osgAnimation::BasicAnimationManager> animationManager = new osgAnimation::BasicAnimationManager;

        osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;

        explicit Player::Player(ContentDb& contentDb, osgViewer::Viewer& viewer);
    };
}

namespace ehb
{
    class IConfig;
    class IFileSys;
    class Region;
    class SiegeNodeMesh;
    class UnveilingTestState: public IGameState
    {
    public:

        UnveilingTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene);

        virtual ~UnveilingTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        IGameStateMgr& gameStateMgr;
        IConfig& config;
        IFileSys& fileSys;
        ContentDb& contentDb;

        osgViewer::Viewer& viewer;
        osg::Group& scene;

        Region* region;

        std::unique_ptr<Player> localPlayer = nullptr;
        std::vector<std::unique_ptr<Player>> remotePlayers;
        
        std::shared_ptr<spdlog::logger> log;
    };

    inline UnveilingTestState::UnveilingTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, ContentDb& contentDb, osgViewer::Viewer& viewer, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), contentDb(contentDb), scene(scene), viewer(viewer)
    {
    }
}
