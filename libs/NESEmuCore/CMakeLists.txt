project(NESEmuCore)

add_library(NESEmuCore)
target_sources(NESEmuCore
    PRIVATE
        src/bus.cpp
        src/cpu6502.cpp
        src/cartridge.cpp
        src/dma.cpp
        src/mapper_nrom.cpp
        src/ppu.cpp
        src/rp2a03.cpp
        src/system.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/NESEmuCore/bus.hpp
            include/NESEmuCore/cartridge.hpp
            include/NESEmuCore/clock.hpp
            include/NESEmuCore/cpu6502.hpp
            include/NESEmuCore/dma.hpp
            include/NESEmuCore/debug_format.hpp
            include/NESEmuCore/emu_types.hpp
            include/NESEmuCore/interrupt_lines.hpp
            include/NESEmuCore/mapper_nrom.hpp
            include/NESEmuCore/memory.hpp
            include/NESEmuCore/ppu.hpp
            include/NESEmuCore/rp2a03.hpp
            include/NESEmuCore/system.hpp
)
