# used to automatically include and build dependencies
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)

FetchContent_Declare(
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
    GIT_TAG master
)

FetchContent_Declare(
    ValveFileVDF
    GIT_REPOSITORY https://github.com/TinyTinni/ValveFileVDF.git
    GIT_TAG master
)

FetchContent_Declare(
    minIni
    GIT_REPOSITORY https://github.com/compuphase/minIni.git
    GIT_TAG master
)

add_subdirectory(spdlog)
add_subdirectory(whereami)
add_subdirectory(ValveFileVDF)
add_subdirectory(minIni)

# speed up reconfiguration by skipping updates from FetchContent
set (FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "" FORCE)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_INCLUDE_PATHS
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src"
    "${FETCHCONTENT_BASE_DIR}/valvefilevdf-src"
    "${FETCHCONTENT_BASE_DIR}/minini-src/dev"
    PARENT_SCOPE
)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_SOURCE_FILES
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src/whereami.c"
    "${FETCHCONTENT_BASE_DIR}/minini-src/dev/minIni.c"
    PARENT_SCOPE
)
