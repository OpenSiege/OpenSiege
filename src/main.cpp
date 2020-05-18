
#include <whereami.h>
#include <spdlog/spdlog.h>

int main(int argc, char * argv[])
{
    const int length = wai_getExecutablePath(nullptr, 0, nullptr);
    std::string exe(length, '\0');
    wai_getExecutablePath(exe.data(), length, nullptr);

    char dateTime[128] = { '\0' };
    std::time_t now = std::time(nullptr);
    strftime(dateTime, sizeof(dateTime), "%m/%d/%Y %I:%M:%S %p", std::localtime(&now));    

    try
    {
        auto log = spdlog::stdout_logger_mt("console");

        log->info("-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-");
        log->info("-== App          : Open Siege ({} - Retail)", exe); // Platform::getExecutablePath()
        log->info("-== Log category : {}", log->name());
        log->info("-== Session      : {}", dateTime);
        log->info("-== Build        : {} (1.11.1.1486 (msqa:2003.07.0202))");
        log->info("-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-");
    }
    catch (std::exception& e)
    {
        spdlog::get("log")->error("could not create {} logger: {}", "console", e.what());
    }

}
