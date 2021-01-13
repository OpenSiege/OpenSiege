
set(GAME_SOURCES
    "cfg/ArgsConfig.cpp"
    "cfg/IniConfig.cpp"
    "cfg/RegistryConfig.cpp"
    "cfg/SteamConfig.cpp"
    "cfg/UserConfig.cpp"
    "cfg/WritableConfig.cpp"

    "state/GameStateMgr.cpp"
    "state/InitState.cpp"
    "state/TestState.cpp"
    "state/test/GasTestState.cpp"
    "state/test/SiegeNodeTestState.cpp"
    "state/test/UITestState.cpp"
    "state/test/RegionTestState.cpp"
    "state/test/AspectMeshTestState.cpp"

    "osg/FileNameMap.cpp"
    "osg/SiegeNodeMesh.cpp"
    "osg/Aspect.cpp"

    "LocalFileSys.cpp"
    "TankFileSys.cpp"

    "world/Region.cpp"

    "ui/ImageFont.cpp"
    "ui/Rect.cpp"
    "ui/NormalizedRect.cpp"
    "ui/WidgetComponent.cpp"
    "ui/Widget.cpp"
    "ui/TextLine.cpp"

    "console/ConsoleSignatureScanner.cpp"
    "console/Console.cpp"

    "StringTool.cpp"
    "Platform.cpp"
    "Game.cpp"
    "main.cpp"
    "ContentDb.cpp"
)

# main open siege target
add_executable(OpenSiege ${EXTERN_SOURCE_FILES} ${CORE_SOURCES} ${GAME_SOURCES})

# force C++17
set_target_properties(OpenSiege PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(OpenSiege PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OPENSCENEGRAPH_INCLUDE_DIRS} ${EXTERN_INCLUDE_PATHS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(OpenSiege PRIVATE ${OPENSCENEGRAPH_LIBRARIES} Threads::Threads "$<$<CXX_COMPILER_ID:GNU>:stdc++fs;${XDGBASEDIR_LIBRARIES}>" SiegeCore spdlog::spdlog)

if(MSVC)
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
	ASSIGN_SOURCE_GROUPS("Core Files" "${CMAKE_SOURCE_DIR}/src/core" ${CORE_SOURCES})
    ASSIGN_SOURCE_GROUPS("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}" ${GAME_SOURCES})
endif()

install(TARGETS OpenSiege RUNTIME DESTINATION bin)