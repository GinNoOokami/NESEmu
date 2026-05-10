project(NESEmuCoreTests)

include(../../cmake/CPM.cmake)

CPMAddPackage("gh:doctest/doctest@2.5.0")
CPMAddPackage("gh:nlohmann/json@3.12.0")

add_executable(NESEmuCoreTests)

target_sources(NESEmuCoreTests
        PRIVATE
            src/test_main.cpp
            src/cartridge_tests.cpp
            src/cpu6502_tests.cpp
            src/ppu_tests.cpp

        PUBLIC FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/cpu_step_tests.hpp
            include/test_memory.hpp
)

target_link_libraries(NESEmuCoreTests
        PRIVATE
            NESEmuCore
            doctest::doctest
            nlohmann_json::nlohmann_json
)

#target_compile_options(NESEmuCoreTests PRIVATE -H -ftime-report)

# Handle JSON step test files and test ROMs
set(TEST_ARCHIVE ${CMAKE_CURRENT_SOURCE_DIR}/data/nes6502.zip)
set(TEST_OUTPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)
set(TEST_STAMP ${TEST_OUTPUT_DATA_DIR}/.extracted.stamp)
set(TEST_OUTPUT_ROM_DIR ${TEST_OUTPUT_DATA_DIR}/rom)

file(GLOB ROM_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/rom/*.nes"
)

add_custom_command(TARGET NESEmuCoreTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_ROM_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ROM_FILES}
        "${TEST_OUTPUT_ROM_DIR}"
        VERBATIM
)

file(MAKE_DIRECTORY ${TEST_OUTPUT_DATA_DIR})

add_custom_command(
        OUTPUT ${TEST_STAMP}
        COMMAND ${CMAKE_COMMAND} -E tar xvf ${TEST_ARCHIVE}
        COMMAND ${CMAKE_COMMAND} -E touch ${TEST_STAMP}
        DEPENDS ${TEST_ARCHIVE}
        WORKING_DIRECTORY ${TEST_OUTPUT_DATA_DIR}
        VERBATIM
)

add_custom_target(extract_test_data DEPENDS ${TEST_STAMP})
add_dependencies(NESEmuCoreTests extract_test_data)

target_compile_definitions(NESEmuCoreTests
        PRIVATE
        TEST_DATA_DIR="${TEST_OUTPUT_DATA_DIR}/nes6502/v1"
)

# Handle testing setup
enable_testing()

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(NESEmuCoreTests)
