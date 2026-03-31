
include(../../cmake/CPM.cmake)
CPMAddPackage(
    NAME SDL2
    URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.10.zip"
    VERSION 2.32.10
)

file(GLOB headers CONFIGURE_DEPENDS include/*.hpp)
file(GLOB sources CONFIGURE_DEPENDS src/*.cpp)

add_executable(NESEmuGUI)

target_sources(NESEmuGUI
    PRIVATE
        ${sources}
)

target_link_libraries(NESEmuGUI
    PRIVATE
        NESEmuCore
        SDL2::SDL2
)
