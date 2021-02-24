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

set(BUILD_TESTING OFF CACHE BOOL "Stop valvevdf from testing" FORCE)

FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(whereami)
FetchContent_MakeAvailable(ValveFileVDF)
FetchContent_MakeAvailable(minIni)

# speed up reconfiguration by skipping updates from FetchContent
set (FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "" FORCE)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_INCLUDE_PATHS
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src"
    "${FETCHCONTENT_BASE_DIR}/valvefilevdf-src"
    "${FETCHCONTENT_BASE_DIR}/minini-src/dev"
    "${CMAKE_CURRENT_SOURCE_DIR}/miniz"
    PARENT_SCOPE
)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_SOURCE_FILES
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src/whereami.c"
    "${FETCHCONTENT_BASE_DIR}/minini-src/dev/minIni.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/miniz/miniz.c"
    PARENT_SCOPE
)

set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "Stop extern updating" FORCE)
