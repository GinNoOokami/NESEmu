project(NESEmuGUI)

include(../../cmake/CPM.cmake)
CPMAddPackage(
        NAME SDL3
        URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.8.zip"
        VERSION 3.4.8
)

add_executable(NESEmuGUI)

target_sources(NESEmuGUI
    PRIVATE
        src/main.cpp
        src/fps.cpp
        src/nesemu_sdl_app.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/NESEmuGUI/app.hpp
            include/NESEmuGUI/fps.hpp
            include/NESEmuGUI/nesemu_sdl_app.hpp
)

target_link_libraries(NESEmuGUI
    PRIVATE
        NESEmuCore
        SDL3::SDL3
)
