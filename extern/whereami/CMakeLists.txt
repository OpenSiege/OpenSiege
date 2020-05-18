FetchContent_GetProperties(whereami)

# whereami doesn't have a cmake build system so dont add_subdirectory - we will include it manually to build
if (NOT whereami_POPULATED)
    message(STATUS "Fetching whereami")
    FetchContent_Populate(whereami)
endif()
