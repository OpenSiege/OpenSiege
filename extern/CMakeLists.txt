# used to automatically include and build dependencies
include(FetchContent)

FetchContent_Declare (
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

FetchContent_Declare (
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
)

FetchContent_Declare (
    ValveFileVDF
    GIT_REPOSITORY https://github.com/TinyTinni/ValveFileVDF.git
)

FetchContent_Declare (
    minIni
    GIT_REPOSITORY https://github.com/compuphase/minIni.git
)

add_subdirectory(spdlog)
add_subdirectory(whereami)
add_subdirectory(ValveFileVDF)
add_subdirectory(minIni)
