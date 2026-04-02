project(NESEmuCoreTests)

include(../../cmake/CPM.cmake)

CPMAddPackage("gh:doctest/doctest@2.5.0")

add_executable(NESEmuCoreTests)

target_sources(NESEmuCoreTests
    PRIVATE
        src/test_main.cpp
        src/cpu_6502_tests.cpp
)

target_link_libraries(NESEmuCoreTests
    PRIVATE
        NESEmuCore
        doctest::doctest
)

enable_testing()

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
#include_directories(${DOCTEST_INCLUDE_DIR})
doctest_discover_tests(NESEmuCoreTests)
