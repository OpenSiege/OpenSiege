
#include "GasTestState.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void GasTestState::enter()
    {
        spdlog::get("log")->info("GasTestState::enter()");
    }

    void GasTestState::leave()
    {
    }

    void GasTestState::update(double deltaTime)
    {
    }

    bool GasTestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
