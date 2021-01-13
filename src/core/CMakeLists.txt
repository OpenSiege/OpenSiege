
find_package(BISON REQUIRED)
find_package(RE2C REQUIRED)

bison_target(FuelParser "gas/FuelParser.y" "${CMAKE_BINARY_DIR}/gas/FuelParser.cpp" COMPILE_FLAGS "${YACC_FLAGS} --defines=${CMAKE_BINARY_DIR}/gas/FuelParser.hpp")
re2c_target("gas/FuelScanner.r2c" "${CMAKE_BINARY_DIR}/gas/FuelScanner.cpp")

# use a static library to ensure gas is built properly
# expand on this in the future?
add_library(SiegeCore STATIC)

target_sources(SiegeCore 
	PRIVATE
		${BISON_FuelParser_OUTPUTS}
		${CMAKE_BINARY_DIR}/gas/FuelScanner.cpp
		"gas/Fuel.cpp"
)

target_include_directories(SiegeCore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}$<SEMICOLON>${CMAKE_BINARY_DIR}>/gas)

# just dump core sources directly into the projects
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/BinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/ReaderWriterRAW.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/ReaderWriterSNO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/ReaderWriterASP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/ReaderWriterFont.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osgPlugins/ReaderWriterSiegeNodeList.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/ui/ImageFont.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/Rect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/NormalizedRect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/WidgetComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/Widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/TextLine.cpp
	
	PARENT_SCOPE
)