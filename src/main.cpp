
#include <spdlog/spdlog.h>

int main(int argc, char * argv[])
{
    auto console = spdlog::stdout_color_st("console");
    console->info("Hello, World!");
}
