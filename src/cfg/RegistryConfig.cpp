
#include <minIni.h>
#include <filesystem>
#include "WritableConfig.hpp"

#ifdef WIN32
    #include <windows.h>
    #include <shlobj.h>
#endif

namespace fs = std::filesystem;

namespace ehb
{
    constexpr const char * registryKeyVec[] =
    {
        "Software\\Wow6432Node\\Microsoft\\Microsoft Games\\Dungeon Siege Legends of Aranna\\1.0",
        "Software\\Wow6432Node\\Microsoft\\Microsoft Games\\DungeonSiege\\1.0",
        "Software\\Wow6432Node\\Microsoft\\Microsoft Games\\DungeonSiegeDemo\\1.0",
        "Software\\Microsoft\\Microsoft Games\\Dungeon Siege Legends of Aranna\\1.0",
        "Software\\Microsoft\\Microsoft Games\\DungeonSiege\\1.0",
        "Software\\Microsoft\\Microsoft Games\\DungeonSiegeDemo\\1.0"
    };

    // TODO: make an IConfigSrc class
    // TODO: convert this into an implementation of the IConfigSrc class
#ifdef WIN32
    void registryConfig(WritableConfig & config)
    {
        for (const char * key : registryKeyVec)
        {
            // http://stackoverflow.com/questions/34065/how-to-read-a-value-from-the-windows-registry
            if (HKEY hKey; RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
            {
                CHAR szBuffer[MAX_PATH];
                DWORD dwBufferSize = sizeof(szBuffer);

                if (RegQueryValueEx(hKey, "EXE Path", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
                {
                    config.setString("ds-install-path", szBuffer);

                    break;
                }
            }
        }
    }
#else
    void registryConfig(WritableConfig & config)
    {
        if (const char * envVar = std::getenv("WINEPREFIX"))
        {
            const fs::path system = fs::path(envVar) / "system.reg";

            if (fs::is_regular_file(system))
            {
                minIni ini(system.string());
                const fs::path devices = fs::path(envVar) / "dosdevices";

                for (std::string section : registryKeyVec)
                {
                    // std::cout << "looking for section: " << section << std::endl;

                    { // replace all \ with \\... because wine is like that
                        std::string::size_type index = 0;

                        while ((index = section.find("\\", index)) != std::string::npos)
                        {
                            section.insert(index, "\\");

                            index += 2;
                        }
                    }

                    std::string value = ini.gets(section, "\"EXE Path\"");

                    if (value.empty())
                    {
                        // couldn't find windows registry key in wine... no problem, app just not installed
                    }
                    else
                    {
                        { // replace all \\ with \... because wine is like that
                            std::string::size_type index = 0;

                            while ((index = value.find("\\\\", index)) != std::string::npos)
                            {
                                value.replace(index, 2, "/");
                            }
                        }

                        // wine uses lowercase drive letters under $WINEPREFIX/dosdevices/
                        value[0] = std::tolower(value[0]);

                        config.setString("ds-install-path", (fs::path(envVar) / "dosdevices" / value).string());

                        break;
                    }
                }
            }
            else
            {
                // TODO: output error about system.reg missing
            }
        }
    }
#endif
}
