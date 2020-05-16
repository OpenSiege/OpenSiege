cmake_minimum_required(VERSION 3.10)
project(OpenSiege)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

# windows specific - DO NOT ENABLE UNLESS ABSOLUTELY NECESSARY
option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option" OFF)

# required packages for minimal build should go here
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgAnimation osgUtil osgText)
find_package(Threads REQUIRED)

add_executable(OpenSiege "src/main.cpp")

set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS})
target_link_libraries(OpenSiege PUBLIC ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>")

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
