
#include "TankTestState.hpp"

#include <sstream>
#include <filesystem>

#include <spdlog/spdlog.h>

#include "cfg/IConfig.hpp"
#include "filesystem/tank_file.hpp"

namespace fs = std::filesystem;

namespace ehb
{
    void TankTestState::enter()
    {
        auto log = spdlog::get("log");

        fs::path dsInstallPath = config.getString("ds-install-path");

        if (!fs::exists(dsInstallPath))
        {
            log->error("Failed to find local directory: {}", dsInstallPath.generic_string());
            return;
        }

        fs::path resources = dsInstallPath / "Resources";
        if (!fs::exists(resources))
        {
            log->error("Failed to find resources directory @ {}", resources.generic_string());
            return;
        }

        log->info("Running tank tests from {}", resources.generic_string());

        std::vector<fs::path> coreTanks = {

            resources / "DevLogic.dsres",
            resources / "Logic.dsres",
            resources / "Objects.dsres",
            resources / "Sound.dsres",
            resources / "Terrain.dsres",
            resources / "Voices.dsres"
        };

        for (const auto& tank : coreTanks)
        {
            if (!fs::exists(tank))
            {
                log->error("Unable to find core tank {} for testing", tank.generic_string());
                return;
            }
        }

        TankFile devLogic; devLogic.openForReading(coreTanks[0].generic_string());
        TankFile logic; logic.openForReading(coreTanks[1].generic_string());
        TankFile objects; objects.openForReading(coreTanks[2].generic_string());
        TankFile sound; sound.openForReading(coreTanks[3].generic_string());
        TankFile terrain; terrain.openForReading(coreTanks[4].generic_string());
        TankFile voices; voices.openForReading(coreTanks[5].generic_string());

        // these are pretty invalid tests since its always comparing disk file size and not what happens when it gets loaded
        // TODO: remove
        if (!validateFileSize(devLogic, 1445)) return;
        if (!validateFileSize(logic, 4206896)) return;
        if (!validateFileSize(objects, 304438568)) return;
        if (!validateFileSize(sound, 185343092)) return;
        if (!validateFileSize(terrain, 410230240)) return;
        if (!validateFileSize(voices, 45951736)) return;
    }

    void TankTestState::leave()
    {
    }

    void TankTestState::update(double deltaTime)
    {
    }

    bool TankTestState::handle(const osgGA::GUIEventAdapter& event, osgGA::GUIActionAdapter& action)
    {
        return false;
    }
}
