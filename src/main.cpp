
#include <filesystem>
#include <whereami.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "cfg/WritableConfig.hpp"
#include "Platform.hpp"
#include "Game.hpp"

// TODO: move to a new Report class
void registerSiegeLogger(const std::string& path, const std::string& name);

int main(int argc, char * argv[])
{
    using namespace ehb;

    // always initialize the console logger first
    auto log = spdlog::stdout_color_mt("log");

    /*
     * this reads the following configs (order is important):
     * argsConfig
     * iniConfig
     * registryConfig
     * steamConfig
     * userConfig
    */
    WritableConfig config(argc, argv);
    config.dump();

    bool hasBits = !config.getString("bits", "").empty();
    bool hasPath = !config.getString("ds-install-path", "").empty();
    if (!hasBits) log->warn("No Bits directory detected.");
    if (!hasPath) log->warn("No DS Install path detected.");

    // TOOD: remove this when the tank system is in place
    if (!hasBits)
    {
        log->error("No Bits directory detected and OpenSiege currently requires it. Please pass the bits path with --bits");

        return 0;
    }

    // we can survive just on bits or just on the path 
    // but if we don't have either then there are no assets to load
    if (!hasBits && !hasPath)
    {
        log->error("No Bits or DS Install Path detected. Please check you have the proper registry keys / steam / WINE configuration. You can also use the command line flag --ds-install path for manual setup.");

        return 0;
    }

    // all loggers should be registered here before the Game class gets instantiated
    registerSiegeLogger(config.getString("logs_path", ""), "filesystem");
    registerSiegeLogger(config.getString("logs_path", ""), "game");
    registerSiegeLogger(config.getString("logs_path", ""), "scene");
    registerSiegeLogger(config.getString("logs_path", ""), "world");

    osg::ref_ptr<Game> game = new Game(config);

    return game->exec();
}

void registerSiegeLogger(const std::string& path, const std::string& name)
{
    char dateTime[128] = { '\0' };
    std::time_t now = std::time(nullptr);
    strftime(dateTime, sizeof(dateTime), "%m/%d/%Y %I:%M:%S %p", std::localtime(&now));

    // TODO: what are the last 4 digits of the msqa
    char msqa[128] = { '\0' };
    strftime(msqa, sizeof(msqa), "%Y.%m.0202", std::localtime(&now));

    std::filesystem::path fullpath(path);
    fullpath = fullpath / std::string(name + ".log");

    try
    {
        auto log = spdlog::basic_logger_mt(name, fullpath.string(), true);

        log->info("-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-");
        log->info("-== App          : Open Siege ({} - Retail)", ehb::Platform::instance().getExecutablePath());
        log->info("-== Log category : {}", log->name());
        log->info("-== Session      : {}", dateTime);
        log->info("-== Build        : [] (1.11.1.1486 (msqa:{}))", msqa);
        log->info("-== Git          : {} ({})", ehb::Platform::Git::GIT_BRANCH, ehb::Platform::Git::GIT_SHA1);
        log->info("-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-");

        auto console = spdlog::get("log")->sinks().back();
        log->sinks().push_back(console);
    }
    catch (std::exception& e)
    {
        spdlog::get("log")->error("could not create {} logger: {}", name, e.what());
    }
}
