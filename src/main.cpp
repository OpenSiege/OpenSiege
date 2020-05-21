
#include <filesystem>
#include <whereami.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "cfg/WritableConfig.hpp"
#include "Platform.hpp"

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

    // all loggers should be registered here before the Game class gets instantiated
    registerSiegeLogger(config.getString("logs_path", ""), "filesystem");
    registerSiegeLogger(config.getString("logs_path", ""), "game");
    registerSiegeLogger(config.getString("logs_path", ""), "scene");
    registerSiegeLogger(config.getString("logs_path", ""), "world");

    return 0;
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
        log->info("-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-");

        auto console = spdlog::get("log")->sinks().back();
        log->sinks().push_back(console);
    }
    catch (std::exception& e)
    {
        spdlog::get("log")->error("could not create {} logger: {}", name, e.what());
    }
}
