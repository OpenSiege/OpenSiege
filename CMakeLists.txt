cmake_minimum_required(VERSION 3.11)
project(OpenSiege)

# used to automatically include and build dependencies
include(FetchContent)

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# windows specific - DO NOT ENABLE UNLESS ABSOLUTELY NECESSARY
option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option" OFF)

add_subdirectory(extern)

# required packages for minimal build should go here
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgAnimation osgUtil osgText)
find_package(Threads REQUIRED)
find_package(spdlog CONFIG QUIET)

# main open siege target
add_executable(OpenSiege "src/main.cpp")

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS})
target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" spdlog::spdlog)

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
