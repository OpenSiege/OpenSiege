
#pragma once

#include "state/IGameState.hpp"

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
    class IConfig;
    class IFileSys;
    class ContentDb;
    class Aspect;
    class AspectMeshTestState final : public IGameState
    {

    public:

        AspectMeshTestState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys& fileSys, osgViewer::Viewer& viewer, ContentDb& contentDb, osg::Group& scene);

        virtual ~AspectMeshTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        void clearAndLoadMesh(const std::string& filename);

    private:

        std::shared_ptr<spdlog::logger> log;

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;
        ContentDb& contentDb;

        osgViewer::Viewer& viewer;
        osg::Group& scene;

        Aspect* mesh = nullptr;
        osg::MatrixTransform* transform = nullptr;

        //! store these while we still use factory BasicAnimationManager
        bool isFidgeting = true;
        osg::ref_ptr<osgAnimation::Animation> fidgetAnimation = nullptr;
        osg::ref_ptr<osgAnimation::Animation> walkAnimation = nullptr;
        osg::ref_ptr<osgAnimation::BasicAnimationManager> animationManager = nullptr;
    };

    inline AspectMeshTestState::AspectMeshTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, osgViewer::Viewer& viewer, ContentDb& contentDb, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), scene(scene), viewer(viewer), contentDb(contentDb)
    {
    }
}
