cmake_minimum_required(VERSION 3.23)
project(NESEmu)

set(CMAKE_CXX_STANDARD 20)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

add_subdirectory(libs/NESEmuCore)
add_subdirectory(apps/NESEmuGUI)
add_subdirectory(test/NESEmuCoreTests)
