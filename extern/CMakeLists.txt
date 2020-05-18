# used to automatically include and build dependencies
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

FetchContent_Declare(
    whereami
    GIT_REPOSITORY https://github.com/gpakosz/whereami.git
)

FetchContent_Declare(
    ValveFileVDF
    GIT_REPOSITORY https://github.com/TinyTinni/ValveFileVDF.git
)

FetchContent_Declare(
    minIni
    GIT_REPOSITORY https://github.com/compuphase/minIni.git
)

add_subdirectory(spdlog)
add_subdirectory(whereami)
add_subdirectory(ValveFileVDF)
add_subdirectory(minIni)

set(EXTERN_INCLUDE_PATHS
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src"
    "${FETCHCONTENT_BASE_DIR}/valvefilevdf-src"
    "${FETCHCONTENT_BASE_DIR}/minIni-src/dev"
    PARENT_SCOPE
)

# the below don't have a cmake build system so we will just manually include them
set(EXTERN_SOURCE_FILES
    "${FETCHCONTENT_BASE_DIR}/whereami-src/src/whereami.c"
    "${FETCHCONTENT_BASE_DIR}/minIni-src/dev/minIni.c"
    PARENT_SCOPE
)
