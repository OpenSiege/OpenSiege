
#include "WritableConfig.hpp"

#include <spdlog/spdlog.h>

namespace ehb
{
    // TODO: make an IConfigSrc class
    // TODO: convert these into implementations of the IConfigSrc class
    //! configuration backend declarations
    void argsConfig(WritableConfig & config, int argc, char * argv[]);
    void iniConfig(WritableConfig & config);
    void registryConfig(WritableConfig & config);
    void steamConfig(WritableConfig & config);
    void userConfig(WritableConfig & config);

    WritableConfig::WritableConfig()
    {
        // load configuration in the specified order
        steamConfig(*this);
        registryConfig(*this);
        iniConfig(*this);
        userConfig(*this);
    }

    WritableConfig::WritableConfig(int argc, char * argv[])
    {
        // load configuration in the specified order
        steamConfig(*this);
        registryConfig(*this);
        iniConfig(*this);
        userConfig(*this);
        argsConfig(*this, argc, argv);
    }

    bool WritableConfig::getBool(const std::string & key, bool defaultValue) const
    {
        const auto itr = boolMap.find(key);

        return itr != boolMap.end() ? itr->second : defaultValue;
    }

    float WritableConfig::getFloat(const std::string & key, float defaultValue) const
    {
        const auto itr = floatMap.find(key);

        return itr != floatMap.end() ? itr->second : defaultValue;
    }

    int WritableConfig::getInt(const std::string & key, int defaultValue) const
    {
        const auto itr = intMap.find(key);

        return itr != intMap.end() ? itr->second : defaultValue;
    }

    const std::string & WritableConfig::getString(const std::string & key, const std::string & defaultValue) const
    {
        const auto itr = stringMap.find(key);

        return itr != stringMap.end() ? itr->second : defaultValue;
    }

    void WritableConfig::dump()
    {
        auto log = spdlog::get("log");

        for (const auto & entry : boolMap)
        {
            log->info("{} = {}", entry.first, entry.second);
        }

        for (const auto & entry : floatMap)
        {
            log->info("{} = {}", entry.first, entry.second);
        }

        for (const auto & entry : intMap)
        {
            log->info("{} = {}", entry.first, entry.second);
        }

        for (const auto & entry : stringMap)
        {
            log->info("{} = {}", entry.first, entry.second);
        }
    }
}
