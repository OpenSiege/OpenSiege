# used to automatically include and build dependencies
include(FetchContent)
include(ExternalProject)

FetchContent_Declare (
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

FetchContent_Declare (
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
)

add_subdirectory(spdlog)
add_subdirectory(whereami)
