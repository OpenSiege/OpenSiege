
#include <osg/ArgumentParser>
#include "WritableConfig.hpp"

namespace ehb
{
    // TODO: make an IConfigSrc class
    // TODO: convert this into an implementation of the IConfigSrc class
    void argsConfig(WritableConfig & config, int argc, char * argv[])
    {
        osg::ArgumentParser args(&argc, argv);

        { // parse all boolean values from the command line
            bool value;

            if (args.read("--fullscreen", value)) config.setBool("fullscreen", value);
            if (args.read("--intro", value)) config.setBool("intro", value);
            if (args.read("--sound", value)) config.setBool("sound", value);
            if (args.read("--textures", value)) config.setBool("drawtextures", value);
        }
        { // parse all float values from the command line
        }
        { // parse all integer values from the command line
            int value;

            if (args.read("--bpp", value)) config.setInt("bpp", value);
            if (args.read("--height", value)) config.setInt("height", value);
            if (args.read("--maxfps", value)) config.setInt("maxfps", value);
            if (args.read("--width", value)) config.setInt("width", value);
        }
        { // parse all string values from the command line
            std::string value;

            if (args.read("--bits", value)) config.setString("bits", value);
            if (args.read("--ds-install-path", value)) config.setString("ds-install-path", value);
            if (args.read("--map_paths", value)) config.setString("map_paths", value);
            if (args.read("--mod_paths", value)) config.setString("mod_paths", value);
            if (args.read("--res_paths", value)) config.setString("res_paths", value);

            if (args.read("--state", value)) config.setString("state", value);
            if (args.read("--log", value)) config.setString("log", value);

            // some temporary debugging params for community testing
            if (args.read("--region", value)) config.setString("region", value);
            if (args.read ("--ds-install-path")) config.setString("ds-install-path", value);
        }
    }
}
