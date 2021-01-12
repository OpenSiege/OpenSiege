
#include "TestState.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void TestState::enter()
    {
        spdlog::get("log")->info("TestState::enter()");
    }

    void TestState::leave()
    {
    }

    void TestState::update(double deltaTime)
    {
    }

    bool TestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
