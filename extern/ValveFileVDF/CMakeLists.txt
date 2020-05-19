FetchContent_GetProperties(ValveFileVDF)

# ValveFileVDF doesn't have a cmake build system so dont add_subdirectory - we will include it manually to build
if (NOT ValveFileVDF_POPULATED)
    message(STATUS "Fetching ValveFileVDF")
    FetchContent_Populate(ValveFileVDF)
endif()
