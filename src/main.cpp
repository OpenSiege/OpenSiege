
#include <filesystem>
#include <whereami.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include "cfg/WritableConfig.hpp"
#include "Platform.hpp"
#include "Game.hpp"

// TODO: move to a new Report class
void registerSiegeLogger(const std::string& path, const std::string& name, spdlog::sinks_init_list slist = {});

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
    if (!hasBits) log->info("No Bits directory detected.");
    if (!hasPath) log->warn("No DS Install path detected.");

    // we can survive just on bits or just on the path 
    // but if we don't have either then there are no assets to load
    if (!hasPath)
    {
        log->error("No DS Install Path detected. Please check you have the proper registry keys / steam / WINE configuration. You can also use the command line flag --ds-install-path for manual setup.");

        return 0;
    }

    // all loggers should be registered here before the Game class gets instantiated
    registerSiegeLogger(config.getString("logs_path", ""), "filesystem");
    registerSiegeLogger(config.getString("logs_path", ""), "game");
    registerSiegeLogger(config.getString("logs_path", ""), "scene");
    registerSiegeLogger(config.getString("logs_path", ""), "world");

    // TODO: put this behind some cmake option to disable unit testing
    registerSiegeLogger(config.getString("logs_path", ""), "testing");

    osg::ref_ptr<Game> game = new Game(config);

    return game->exec();
}

void registerSiegeLogger(const std::string& path, const std::string& name, spdlog::sinks_init_list slist)
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

        // get our console sink which is created by default
        auto console = spdlog::get("log")->sinks().back();

        // make sure all logs also print out to our console
        log->sinks().push_back(console);

        // add any custom sinks passed in to the function, should the console be passed in for clarity?
        for (auto sink : slist)
        {
            log->sinks().push_back(sink);
        }
    }
    catch (std::exception& e)
    {
        spdlog::get("log")->error("could not create {} logger: {}", name, e.what());
    }
}
