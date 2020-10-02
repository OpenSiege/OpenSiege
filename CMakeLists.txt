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
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgText osgUtil)
find_package(Threads REQUIRED)
find_package(spdlog CONFIG QUIET)
find_package(BISON REQUIRED)
find_package(RE2C REQUIRED)

bison_target(FuelParser "${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelParser.y" "${CMAKE_CURRENT_BINARY_DIR}/FuelParser.cpp" COMPILE_FLAGS "${YACC_FLAGS} --defines=${CMAKE_CURRENT_BINARY_DIR}/FuelParser.hpp")
re2c_target("${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelScanner.r2c" "${CMAKE_CURRENT_BINARY_DIR}/FuelScanner.cpp")

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
    "src/state/test/GasTestState.cpp"
    "src/state/test/SiegeNodeTestState.cpp"
    "src/state/test/UITestState.cpp"
    "src/state/test/RegionTestState.cpp"

    ${BISON_FuelParser_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/FuelScanner.cpp
    "src/gas/Fuel.cpp"

    "src/osg/FileNameMap.cpp"
    "src/osg/SiegeNodeMesh.cpp"

    "src/osgPlugins/BinaryReader.cpp"
    "src/osgPlugins/ReaderWriterRAW.cpp"
    "src/osgPlugins/ReaderWriterSNO.cpp"
    "src/osgPlugins/ReaderWriterFont.cpp"
    "src/osgPlugins/ReaderWriterSiegeNodeList.cpp"

    "src/LocalFileSys.cpp"
    "src/TankFileSys.cpp"

    "src/world/Region.cpp"

    "src/ui/ImageFont.cpp"
    "src/ui/Rect.cpp"
    "src/ui/NormalizedRect.cpp"
    "src/ui/WidgetComponent.cpp"
    "src/ui/Widget.cpp"
    "src/ui/TextLine.cpp"

    "src/console/ConsoleSignatureScanner.cpp"
    "src/console/Console.cpp"

    "src/StringTool.cpp"
    "src/Platform.cpp"
    "src/Game.cpp"
    "src/main.cpp"
)

# main open siege target
add_executable(OpenSiege ${EXTERN_SOURCE_FILES} ${SIEGE_SOURCES})

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS} ${EXTERN_INCLUDE_PATHS} ${CMAKE_CURRENT_BINARY_DIR})
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
    ASSIGN_SOURCE_GROUPS("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}" ${SIEGE_SOURCES})
endif()

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
