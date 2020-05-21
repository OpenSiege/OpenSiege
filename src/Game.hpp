
#pragma once

#include <osgGA/GUIEventHandler>
#include "state/IGameStateProvider.hpp"
#include "state/GameStateMgr.hpp"

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
    };
}
