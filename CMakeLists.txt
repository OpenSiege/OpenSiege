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


# global options for all projects
if(MSVC)
    if(DISABLE_MSVC_DEBUG_ITERATOR)
        add_definitions("-D_ITERATOR_DEBUG_LEVEL=0")
    endif(DISABLE_MSVC_DEBUG_ITERATOR)
    add_compile_options("/MP")
endif(MSVC)

add_subdirectory(src)