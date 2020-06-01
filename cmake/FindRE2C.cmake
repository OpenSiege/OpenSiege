# - Find re2c executable and provides macros to generate custom build rules
# The module defines the following variables:
#
#  RE2C_EXECUTABLE - path to the bison program
#  RE2C_VERSION - version of bison
#  RE2C_FOUND - true if the program was found
#

find_program(RE2C_EXECUTABLE NAMES re2c DOC "path to re2c executable")

if(RE2C_EXECUTABLE)

    execute_process(COMMAND ${RE2C_EXECUTABLE} --vernum
        RESULT_VARIABLE RE2C_version_result
        OUTPUT_VARIABLE RE2C_version_output
        ERROR_VARIABLE RE2C_version_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    macro(re2c_target Re2cInput Re2cOutput)
        add_custom_command(OUTPUT ${Re2cOutput}
            COMMAND ${RE2C_EXECUTABLE} -s -o ${Re2cOutput} ${Re2cInput}
            VERBATIM
            DEPENDS ${Re2cInput}
            COMMENT "[RE2C] Building re2c scanner with re2c ${RE2C_VERSION}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endmacro(re2c_target)
endif(RE2C_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RE2C DEFAULT_MSG RE2C_EXECUTABLE)