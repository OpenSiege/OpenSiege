
#include "FullMapTestState.hpp"

namespace ehb
{
    void FullMapTestState::enter()
    {
        log = spdlog::get("log");

        log->info("FullMapTestState::enter()");
    }

    void FullMapTestState::leave()
    {
    }

    void FullMapTestState::update(double deltaTime)
    {
    }

    bool FullMapTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
