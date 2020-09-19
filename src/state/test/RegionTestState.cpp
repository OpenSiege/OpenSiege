
#include "RegionTestState.hpp"

#include "IFileSys.hpp"
#include "gas/Fuel.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    void RegionTestState::enter()
    {
        auto log = spdlog::get("log");
        log->info("RegionTestState::enter()");

        static const std::string directory = "/world/global/siege_nodes";

        fileSys.eachGasFile(directory, [this, log](const std::string& filename, auto doc)
        {
            log->info("reading gas file");
        });
    }

    void RegionTestState::leave()
    {
    }

    void RegionTestState::update(double deltaTime)
    {
    }

    bool RegionTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
