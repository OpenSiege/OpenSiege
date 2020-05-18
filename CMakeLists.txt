cmake_minimum_required(VERSION 3.11)
project(OpenSiege)

# used to automatically include and build dependencies
include(ExternalProject)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

if (UNIX)
    find_package(PkgConfig)
    pkg_check_modules(XDGBASEDIR REQUIRED libxdg-basedir)
endif(UNIX)

# windows specific - DO NOT ENABLE UNLESS ABSOLUTELY NECESSARY
option(DISABLE_MSVC_DEBUG_ITERATOR "Disable the debug iterator for MSVC. All libraries must be built with the same option" OFF)

# required packages for minimal build should go here
find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgViewer osgGA osgAnimation osgUtil osgText)
find_package(Threads REQUIRED)
find_package(spdlog CONFIG QUIET)

add_executable(OpenSiege "src/main.cpp")

set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if(spdlog_FOUND)
	add_library(spdlog_ext INTERFACE)
else()
    message(STATUS "Downloading and building spdlog")
	ExternalProject_Add(spdlog_ext
		GIT_REPOSITORY https://github.com/gabime/spdlog.git
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/spdlog
			-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
		BUILD_IN_SOURCE 1
		UPDATE_COMMAND ""
	)

	set(spdlog_DIR ${CMAKE_CURRENT_BINARY_DIR}/spdlog_ext-prefix/src/spdlog_ext CACHE PATH "Path to internally build spdlog" FORCE)
endif()

add_dependencies(OpenSiege spdlog_ext)

target_include_directories(OpenSiege PUBLIC src ${OPENSCENEGRAPH_INCLUDE_DIRS})
target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" spdlog::spdlog)

install(TARGETS OpenSiege RUNTIME DESTINATION bin)
