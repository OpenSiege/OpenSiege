
#include <minIni.h>
#include <filesystem>
#include "WritableConfig.hpp"

#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace ehb
{
    // TODO: make an IConfigSrc class
    // TODO: convert this into an implementation of the IConfigSrc class
    void iniConfig(WritableConfig & config)
    {
        const fs::path iniFileName = fs::path(config.getString("config-dir", ".")) / "OpenSiege.ini";

        if (fs::is_regular_file(iniFileName))
        {
            minIni ini(iniFileName.string());
            { // parse all integer values from user configuration file
                int value;

                if ((value = ini.geti("", "x", -1)) != -1) config.setInt("x", value);
                if ((value = ini.geti("", "y", -1)) != -1) config.setInt("y", value);
                if ((value = ini.geti("", "width", -1)) != -1) config.setInt("width", value);
                if ((value = ini.geti("", "height", -1)) != -1) config.setInt("height", value);
                if ((value = ini.geti("", "bpp", -1)) != -1) config.setInt("bpp", value);
            }

            { // parse all string values from user configuration file

                // bits can be a bit weird. for now we are going to let the command line take
                // priority if its been set. the ini should just be a fall back if the command line is empty
                if (config.getString("bits", "").empty()) config.setString("bits", ini.gets("OpenSiege", "bits", ""));
            }
        }
    }
}
