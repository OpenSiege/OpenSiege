
#include "ExitState.hpp"

#include <osgViewer/Viewer>

namespace ehb
{
    void ExitState::enter()
    {
        viewer.setDone(true);
    }

    void ExitState::leave()
    {
    }

    void ExitState::update(double deltaTime)
    {
    }

    bool ExitState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
