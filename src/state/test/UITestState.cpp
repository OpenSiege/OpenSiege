
#include "UITestState.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void UITestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("UITestState::enter()");
    }

    void UITestState::leave()
    {
    }

    void UITestState::update(double deltaTime)
    {
    }

    bool UITestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
