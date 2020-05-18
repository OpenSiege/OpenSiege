FetchContent_GetProperties(minIni)

# minIni doesn't have a cmake build system so dont add_subdirectory - we will include it manually to build
if (NOT minIni_POPULATED)
    message(STATUS "Fetching minIni")
    FetchContent_Populate(minIni)
endif()
