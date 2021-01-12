
#pragma once

#include <osgDB/ReaderWriter>

namespace osgGA
{
    class GUIActionAdapter;
    class GUIEventAdapter;
}

namespace ehb
{
    class IGameStateMgr;
    class IGameState
    {
    public:

        virtual ~IGameState() = default;

        virtual void enter() { }
        virtual void leave() { }
        virtual void update(double deltaTime) { }
        virtual bool handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action) { return false; }
    };
}
