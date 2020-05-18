FetchContent_GetProperties(whereami)

if (NOT whereami_POPULATED)
    message(STATUS "Fetching whereami")
    FetchContent_Populate(whereami)
    add_subdirectory(${whereami_SOURCE_DIR} ${whereami_BINARY_DIR})
endif()
