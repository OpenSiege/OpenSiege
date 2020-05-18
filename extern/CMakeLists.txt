# used to automatically include and build dependencies
include(FetchContent)
include(ExternalProject)

FetchContent_Declare (
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

ExternalProject_Add (
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

add_subdirectory(spdlog)
