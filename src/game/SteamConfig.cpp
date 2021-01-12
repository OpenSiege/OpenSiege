
#include "WritableConfig.hpp"
#include <vector>
#include <vdf_parser.hpp>
#include <filesystem>

namespace fs = std::filesystem;

namespace ehb
{
    //! DS1, without LOA: 39190
    //! DS2, expansion? 39200

    // TODO: make an IConfigSrc class
    // TODO: convert this into an implementation of the IConfigSrc class

    void locate_install_dir(WritableConfig& config, const fs::path & steamApps);

#ifdef WIN32

    #include <windows.h>

    constexpr const char* registryKey = "Software\\Valve\\Steam";

    void steamConfig(WritableConfig & config)
    {
        if (HKEY hKey; RegOpenKeyEx(HKEY_CURRENT_USER, registryKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            CHAR szBuffer[MAX_PATH];
            DWORD dwBufferSize = sizeof(szBuffer);

            if (RegQueryValueEx(hKey, "SteamPath", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
            {
                const fs::path steamApps = fs::path(szBuffer) / "steamapps";

                locate_install_dir(config, steamApps);
            }
        }
    }
#else
    void steamConfig(WritableConfig & config)
    {
        if (const char * envVar = std::getenv("HOME"))
        {
            const fs::path steamApps = fs::path(envVar) / ".steam/steam/steamapps";

            locate_install_dir(config, steamApps);
        }
    }
#endif

    void locate_install_dir(WritableConfig& config, const fs::path& steamApps)
    {
        // list of steamapps directories to search for an installed dungeon siege
        std::vector<fs::path> eachSteamApps = { steamApps };

        // check if there are any additional steam library folders and search through them as well
        std::ifstream file(steamApps / "libraryfolders.vdf");

        if (file.is_open())
        {
            auto root = tyti::vdf::read(file);

            for (const auto& entry : root.attribs)
            {
                const std::string& key = entry.first, & value = entry.second;

                /*
                 * steam will list additional library folders starting from "1" like so:
                 * - "1" "/opt"
                 * - "2" "/path/to/library"
                 */
                if (std::all_of(key.begin(), key.end(), ::isdigit))
                {
                    eachSteamApps.emplace_back(fs::path(value) / "steamapps");
                }
            }
        }

        // iterate through full list of library folders searching for DS
        for (const auto& path : eachSteamApps)
        {
            const fs::path installDir = path / "common/Dungeon Siege 1";

            if (fs::is_directory(installDir))
            {
                config.setString("ds-install-path", installDir.string());

                break;
            }
        }
    }
}
