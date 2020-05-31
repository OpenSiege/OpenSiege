FetchContent_GetProperties(spdlog)

if (NOT spdlog_POPULATED)
    message(STATUS "Fetching spdlog")
    FetchContent_Populate(spdlog)
    set(SPDLOG_BUILD_TESTING CACHE BOOL FALSE)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)

    if(MSVC)
        if(DISABLE_MSVC_DEBUG_ITERATOR)
            target_compile_definitions(spdlog PRIVATE "_ITERATOR_DEBUG_LEVEL=0")
        endif()

        target_compile_options(spdlog PRIVATE "/MP")
    endif()
endif()
