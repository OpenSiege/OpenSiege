cmake_minimum_required(VERSION 3.11)
project(OpenSiege)

# add custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

# windows specific - DO NOT ENABLE UNLESS ABSOLUTELY NECESSARY
option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option" OFF)

add_subdirectory(extern)

# required packages for minimal build should go here
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgText osgUtil osgAnimation)
find_package(Threads REQUIRED)
find_package(spdlog CONFIG QUIET)
find_package(BISON REQUIRED)
find_package(RE2C REQUIRED)

bison_target(FuelParser "${CMAKE_CURRENT_LIST_DIR}/src/game/gas/FuelParser.y" "${CMAKE_CURRENT_BINARY_DIR}/FuelParser.cpp" COMPILE_FLAGS "${YACC_FLAGS} --defines=${CMAKE_CURRENT_BINARY_DIR}/FuelParser.hpp")
re2c_target("${CMAKE_CURRENT_LIST_DIR}/src/game/gas/FuelScanner.r2c" "${CMAKE_CURRENT_BINARY_DIR}/FuelScanner.cpp")

set(GAME_SOURCES
    "src/game/cfg/ArgsConfig.cpp"
    "src/game/cfg/IniConfig.cpp"
    "src/game/cfg/RegistryConfig.cpp"
    "src/game/cfg/SteamConfig.cpp"
    "src/game/cfg/UserConfig.cpp"
    "src/game/cfg/WritableConfig.cpp"

    "src/game/state/GameStateMgr.cpp"
    "src/game/state/InitState.cpp"
    "src/game/state/TestState.cpp"
    "src/game/state/test/GasTestState.cpp"
    "src/game/state/test/SiegeNodeTestState.cpp"
    "src/game/state/test/UITestState.cpp"
    "src/game/state/test/RegionTestState.cpp"
    "src/game/state/test/AspectMeshTestState.cpp"

    ${BISON_FuelParser_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/FuelScanner.cpp
    "src/game/gas/Fuel.cpp"

    "src/game/osg/FileNameMap.cpp"
    "src/game/osg/SiegeNodeMesh.cpp"
    "src/game/osg/Aspect.cpp"

    "src/game/osgPlugins/BinaryReader.cpp"
    "src/game/osgPlugins/ReaderWriterRAW.cpp"
    "src/game/osgPlugins/ReaderWriterSNO.cpp"
    "src/game/osgPlugins/ReaderWriterASP.cpp"
    "src/game/osgPlugins/ReaderWriterFont.cpp"
    "src/game/osgPlugins/ReaderWriterSiegeNodeList.cpp"

    "src/game/LocalFileSys.cpp"
    "src/game/TankFileSys.cpp"

    "src/game/world/Region.cpp"

    "src/game/ui/ImageFont.cpp"
    "src/game/ui/Rect.cpp"
    "src/game/ui/NormalizedRect.cpp"
    "src/game/ui/WidgetComponent.cpp"
    "src/game/ui/Widget.cpp"
    "src/game/ui/TextLine.cpp"

    "src/game/console/ConsoleSignatureScanner.cpp"
    "src/game/console/Console.cpp"

    "src/game/StringTool.cpp"
    "src/game/Platform.cpp"
    "src/game/Game.cpp"
    "src/game/main.cpp"
    "src/game/ContentDb.cpp"
)

# main open siege target
add_executable(OpenSiege ${EXTERN_SOURCE_FILES} ${GAME_SOURCES})

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC src/game ${OPENSCENEGRAPH_INCLUDE_DIRS} ${EXTERN_INCLUDE_PATHS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" spdlog::spdlog)

if(MSVC)
    if(DISABLE_MSVC_DEBUG_ITERATOR)
        target_compile_definitions(OpenSiege PRIVATE "_ITERATOR_DEBUG_LEVEL=0")
    endif()

    target_compile_options(OpenSiege PRIVATE "/MP")

    # place header and source files into group folders to help IDE's present the files in a logical manner
    function(ASSIGN_SOURCE_GROUPS GROUP_NAME ROOT_FOLDER)
        foreach(FILE IN ITEMS ${ARGN})
            if (IS_ABSOLUTE "${FILE}")
                file(RELATIVE_PATH RELATIVE_SOURCE "${ROOT_FOLDER}" "${FILE}")
            else()
                set(RELATIVE_SOURCE "${FILE}")
            endif()
            get_filename_component(SOURCE_PATH "${RELATIVE_SOURCE}" PATH)
            string(REPLACE "/" "\\" SOURCE_PATH_MSVC "${SOURCE_PATH}")
            source_group("${GROUP_NAME}\\${SOURCE_PATH_MSVC}" FILES "${FILE}")
        endforeach()
    endfunction(ASSIGN_SOURCE_GROUPS)

    # enable folders for MSVC
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    ASSIGN_SOURCE_GROUPS("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}" ${GAME_SOURCES})
endif()

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
