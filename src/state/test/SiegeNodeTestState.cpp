
#include "SiegeNodeTestState.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

namespace ehb
{
    void SiegeNodeTestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("SiegeNodeTestState::enter()");

        auto stream = std::make_unique<std::stringstream>();
        *stream << "[t:template,n:actor]{doc=\"Generic brained objects\";}";

        if (Fuel doc; doc.load(*stream))
        {
            assert(doc.child("actor")->name() == "actor");
            assert(doc.child("actor")->type() == "template");
        }
    }

    void SiegeNodeTestState::leave()
    {
    }

    void SiegeNodeTestState::update(double deltaTime)
    {
    }

    bool SiegeNodeTestState::handle(const osgGA::GUIEventAdapter & event, osgGA::GUIActionAdapter & action)
    {
        return false;
    }
}
