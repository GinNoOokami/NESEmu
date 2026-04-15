project(NESEmuCore)

add_library(NESEmuCore)
target_sources(NESEmuCore
    PRIVATE
        src/bus.cpp
        src/cpu6502.cpp
        src/cartridge.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/NESEmuCore/bus.hpp
            include/NESEmuCore/bus_mappable.hpp
            include/NESEmuCore/cartridge.hpp
            include/NESEmuCore/cpu6502.hpp
            include/NESEmuCore/debug_format.hpp
            include/NESEmuCore/emu_types.hpp
            include/NESEmuCore/memory.hpp
)
