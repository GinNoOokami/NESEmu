project(NESEmuCoreTests)

include(../../cmake/CPM.cmake)

CPMAddPackage("gh:doctest/doctest@2.5.0")
CPMAddPackage("gh:nlohmann/json@3.12.0")

add_executable(NESEmuCoreTests
    include/cpu_step_tests.hpp)

target_sources(NESEmuCoreTests
    PRIVATE
        src/test_main.cpp
        src/cpu_6502_tests.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/cpu_step_tests.hpp
)

target_link_libraries(NESEmuCoreTests
    PRIVATE
        NESEmuCore
        doctest::doctest
        nlohmann_json::nlohmann_json
)

enable_testing()

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(NESEmuCoreTests)
