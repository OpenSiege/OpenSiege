
#include "WritableConfig.hpp"

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <cassert>

#ifdef WIN32
    #include <windows.h>
    #include <shlobj.h>
#else
    #include <basedir.h>
#endif

namespace ehb
{
    // TODO: make an IConfigSrc class
    // TODO: convert this into an implementation of the IConfigSrc class
    void userConfig(WritableConfig & config)
    {
#ifdef WIN32
        { // grab the win32 specific folders to use for cache, config, and user data
            CHAR path[MAX_PATH];

            if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
            {
                const std::string value = osgDB::concatPaths(path, "OpenSiege");

                if (osgDB::makeDirectory(value)) config.setString("cache-dir", value);
            }

            if (SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
            {
                const std::string value = osgDB::concatPaths(path, "OpenSiege");

                if (osgDB::makeDirectory(value)) config.setString("config-dir", value);
                if (osgDB::makeDirectory(value)) config.setString("data-dir", value);
            }
        }
#else
        { // grab the freedesktop.org specific folders to use for cache, config, and user data
            xdgHandle xdg;

            xdgInitHandle(&xdg);

            if (const char * path = xdgCacheHome(&xdg))
            {
                const std::string value = osgDB::concatPaths(path, "OpenSiege");

                if (osgDB::makeDirectory(value)) config.setString("cache-dir", value);
            }

            if (const char * path = xdgConfigHome(&xdg))
            {
                const std::string value = osgDB::concatPaths(path, "OpenSiege");

                if (osgDB::makeDirectory(value)) config.setString("config-dir", value);
            }

            if (const char * path = xdgDataHome(&xdg))
            {
                const std::string value = osgDB::concatPaths(path, "OpenSiege");

                if (osgDB::makeDirectory(value)) config.setString("data-dir", value);
            }

            xdgWipeHandle(&xdg);
        }
#endif

        // platform independent folders and setup

        assert(!config.getString("data-dir", "").empty());
        assert(!config.getString("config-dir", "").empty());
        assert(!config.getString("cache-dir", "").empty());

        auto data = config.getString("data-dir", "");

        auto logs = osgDB::concatPaths(data, "Logs");
        auto shots = osgDB::concatPaths(data, "Screen Shots");
        auto mods = osgDB::concatPaths(data, "Mods");

        if (osgDB::makeDirectory(logs)) config.setString("logs_path", logs);
        if (osgDB::makeDirectory(shots)) config.setString("shots_path", shots);
        if (osgDB::makeDirectory(mods)) config.setString("mods_path", mods);
    }
}
