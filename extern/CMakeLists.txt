# used to automatically include and build dependencies
include(FetchContent)

# we are not importing ValveFileVDF here as it causes a test
# project to be imported

FetchContent_Declare(
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
    GIT_TAG master
)

FetchContent_Declare(
    minIni
    GIT_REPOSITORY https://github.com/compuphase/minIni.git
    GIT_TAG master
)

FetchContent_MakeAvailable(whereami)
FetchContent_MakeAvailable(minIni)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_INCLUDE_PATHS
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src"
    "${FETCHCONTENT_BASE_DIR}/minini-src/dev"
    "${CMAKE_CURRENT_SOURCE_DIR}/ValveFileVDF"
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

# speed up reconfiguration by skipping updates from FetchContent
set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "Stop extern updating" FORCE)
