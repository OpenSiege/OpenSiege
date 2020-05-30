cmake_minimum_required(VERSION 3.11)
project(OpenSiege)

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# windows specific - DO NOT ENABLE UNLESS ABSOLUTELY NECESSARY
option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option" OFF)

add_subdirectory(extern)

# required packages for minimal build should go here
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgAnimation osgUtil osgText)
find_package(Threads REQUIRED)
find_package(spdlog CONFIG QUIET)

set(SIEGE_SOURCES
    "src/cfg/ArgsConfig.cpp"
    "src/cfg/IniConfig.cpp"
    "src/cfg/RegistryConfig.cpp"
    "src/cfg/SteamConfig.cpp"
    "src/cfg/UserConfig.cpp"
    "src/cfg/WritableConfig.cpp"

    "src/state/GameStateMgr.cpp"
    "src/state/InitState.cpp"
    "src/state/TestState.cpp"

    "src/Platform.cpp"
    "src/Game.cpp"
    "src/main.cpp"
)

# main open siege target
add_executable(OpenSiege ${EXTERN_SOURCE_FILES} ${SIEGE_SOURCES})

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS} ${EXTERN_INCLUDE_PATHS})
target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" spdlog::spdlog)

if(MSVC)
    if(DISABLE_MSVC_DEBUG_ITERATOR)
        target_compile_definitions(OpenSiege PRIVATE "_ITERATOR_DEBUG_LEVEL=0")
    endif()
endif()

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
