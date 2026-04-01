project(NESEmuCore)

add_library(NESEmuCore)
target_sources(NESEmuCore
    PRIVATE
        src/cpu_6502.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/NESEmuCore/cpu_6502.hpp
            include/NESEmuCore/emu_types.hpp
)
