
#include "InitState.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void InitState::enter()
    {
        spdlog::get("log")->info("InitState::enter()");
    }

    void InitState::leave()
    {
    }

    void InitState::update(double deltaTime)
    {
    }

    bool InitState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
