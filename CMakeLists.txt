cmake_minimum_required(VERSION 3.11)
project(OpenSiege)

# add custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option. This will also turn off Conan." OFF)

# if we want to disable debug iterators then we need to make sure Conan doesn't do its thing
# this should probably be fixed in all the upstream packages to include a debug iterator flag
if(DISABLE_MSVC_DEBUG_ITERATOR)

    # make sure we set the option globally for all imported projects since some will be built during compilation
    add_compile_definitions("_ITERATOR_DEBUG_LEVEL=0")

    # required since we are no longer using Conan to find our dependencies
    find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgText osgAnimation osgUtil)
    find_package(Threads REQUIRED)
    
    # quiet config since we will be downloading and building from scratch
    find_package(spdlog CONFIG QUIET)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

add_subdirectory(extern)

option(REGENERATE_FUEL_PARSER "Enable this option to regenerate the Fuel parser files" OFF)

if(REGENERATE_FUEL_PARSER)
    # these are required due to having exes associated with them
    find_package(BISON REQUIRED)
    find_package(RE2C REQUIRED)

    bison_target(FuelParser "${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelParser.y" "${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelParser.cpp" COMPILE_FLAGS "${YACC_FLAGS} --no-lines --defines=${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelParser.hpp")
    re2c_target(NAME FuelScanner INPUT "${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelScanner.r2c" OUTPUT "${CMAKE_CURRENT_LIST_DIR}/src/gas/FuelScanner.cpp" OPTIONS "--no-debug-info")
endif()

execute_process(
	COMMAND git rev-parse --abbrev-ref HEAD
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_BRANCH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_SHA1
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file("${CMAKE_SOURCE_DIR}/src/Platform.cpp.in" "${CMAKE_BINARY_DIR}/Platform.cpp" @ONLY)

set(SIEGE_SOURCES
    "src/cfg/ArgsConfig.cpp"
    "src/cfg/IniConfig.cpp"
    "src/cfg/RegistryConfig.cpp"
    "src/cfg/SteamConfig.cpp"
    "src/cfg/UserConfig.cpp"
    "src/cfg/WritableConfig.cpp"

    "src/state/GameStateMgr.cpp"
    "src/state/InitState.cpp"
	"src/state/ExitState.cpp"
    "src/state/TestState.cpp"
    "src/state/test/GasTestState.cpp"
    "src/state/test/SiegeNodeTestState.cpp"
    "src/state/test/UITestState.cpp"
    "src/state/test/RegionTestState.cpp"
    "src/state/test/AspectMeshTestState.cpp"
    "src/state/test/TankTestState.cpp"

    "src/gas/FuelScanner.cpp"
    "src/gas/FuelParser.cpp"
    "src/gas/Fuel.cpp"

    "src/osg/FileNameMap.cpp"
    "src/osg/SiegeNodeMesh.cpp"
    "src/osg/Aspect.cpp"
    "src/osg/PRS.cpp"

    "src/osgPlugins/BinaryReader.cpp"
    "src/osgPlugins/ReaderWriterRAW.cpp"
    "src/osgPlugins/ReaderWriterSNO.cpp"
    "src/osgPlugins/ReaderWriterASP.cpp"
    "src/osgPlugins/ReaderWriterPRS.cpp"
    "src/osgPlugins/ReaderWriterFont.cpp"
    "src/osgPlugins/ReaderWriterSiegeNodeList.cpp"
    "src/osgPlugins/ReaderWriterUI.cpp"

    "src/filesystem/TankFile.cpp"
    "src/filesystem/TankFileReader.cpp"
    "src/filesystem/LocalFileSys.cpp"
    "src/filesystem/TankFileSys.cpp"

    "src/world/Region.cpp"

    "src/ui/ImageFont.cpp"
    "src/ui/Rect.cpp"
    "src/ui/NormalizedRect.cpp"
    "src/ui/WidgetComponent.cpp"
    "src/ui/Widget.cpp"
    "src/ui/TextLine.cpp"
    "src/ui/Button.cpp"
    "src/ui/DialogBox.cpp"
    "src/ui/Justification.cpp"
    "src/ui/Text.cpp"
    "src/ui/Shell.cpp"

    "src/console/ConsoleSignatureScanner.cpp"
    "src/console/Console.cpp"

    "src/StringTool.cpp"
    "${CMAKE_BINARY_DIR}/Platform.cpp"
    "src/Game.cpp"
    "src/main.cpp"
    "src/ContentDb.cpp"

    # TEMP
    "src/GodDI.cpp"
)

# main open siege target
add_executable(OpenSiege ${EXTERN_SOURCE_FILES} ${SIEGE_SOURCES})

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if(MSVC)
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

if(DISABLE_MSVC_DEBUG_ITERATOR)
    target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS} ${EXTERN_INCLUDE_PATHS} ${CMAKE_CURRENT_BINARY_DIR}/fuel)
    target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" spdlog::spdlog)
else()
    target_include_directories(OpenSiege PUBLIC src ${EXTERN_INCLUDE_PATHS} ${CMAKE_CURRENT_BINARY_DIR}/fuel)
    target_link_libraries(OpenSiege PRIVATE CONAN_PKG::openscenegraph CONAN_PKG::spdlog)
endif()

install(TARGETS OpenSiege RUNTIME DESTINATION bin)

# Should the below be an actual CMake target that is launched via some cpp code? this seems pretty nasty
# We could use existing registry lookups to populate all this
option(BUILD_DSMOD_TARGET "Setup a target to launch DSMOD with custom bits path" OFF)
if(BUILD_DSMOD_TARGET)
    set(DS_INSTALL_PATH "<path>" CACHE STRING "Path to DS Install directory")
    set(DSMOD_PATH "<path>" CACHE STRING "Path to DSMod executable")
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/DSModUserPath)
    execute_process(COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/cmake/GenerateDSModShortcut.ps1 ${DS_INSTALL_PATH} ${DSMOD_PATH} ${CMAKE_SOURCE_DIR}/bits
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
