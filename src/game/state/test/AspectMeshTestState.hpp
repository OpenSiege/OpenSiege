
#pragma once

#include "state/IGameState.hpp"

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
    class Aspect;
    class AspectMeshTestState : public IGameState
    {

        const std::vector<std::string> farmgirl = {

            "m_c_gah_fg_pos_a1.asp",
            "m_c_gah_fg_pos_a2.asp",
            "m_c_gah_fg_pos_a3.asp",
            "m_c_gah_fg_pos_a4.asp",
            "m_c_gah_fg_pos_a5.asp",
            "m_c_gah_fg_pos_a6.asp",
            "m_c_gah_fg_pos_a7.asp",
        };

    public:

        AspectMeshTestState(IGameStateMgr & gameStateMgr, IConfig & config, IFileSys& fileSys, osgViewer::Viewer& viewer, osg::Group& scene);

        virtual ~AspectMeshTestState() = default;

        void enter() override;
        void leave() override;
        void update(double deltaTime) override;
        bool handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action) override;

    private:

        void clearAndLoadMesh(const std::string& filename);

    private:

        IGameStateMgr & gameStateMgr;
        IConfig & config;
        IFileSys& fileSys;

        osgViewer::Viewer& viewer;
        osg::Group& scene;

        Aspect* mesh = nullptr;
        osg::MatrixTransform* transform = nullptr;
    };

    inline AspectMeshTestState::AspectMeshTestState(IGameStateMgr& gameStateMgr, IConfig& config, IFileSys& fileSys, osgViewer::Viewer& viewer, osg::Group& scene) : gameStateMgr(gameStateMgr), config(config), fileSys(fileSys), scene(scene), viewer(viewer)
    {
    }
}
