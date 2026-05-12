#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/clock.hpp"
#include "NESEmuCore/interrupt_lines.hpp"
#include "NESEmuCore/memory.hpp"
#include "NESEmuCore/ppu.hpp"
#include "NESEmuCore/rp2a03.hpp"

#include <doctest.h>

using namespace NESEmu;

TEST_SUITE("DMA Tests") {
TEST_CASE("OAM DMA")
{
    SUBCASE("writing $4014 initiates DMA transfer") {
        static constexpr uint16 kOamAddrRegister = 0x2003;
        static constexpr uint16 kOamDataRegister = 0x2004;
        static constexpr uint16 kOamDmaRegister  = 0x4014;

        Clock          clock;
        WorkRam        memory;
        MainBus        mainBus;
        PpuBus         ppuBus;
        InterruptLines interruptLines;
        Rp2A03         cpu(clock, mainBus, interruptLines);
        Ppu            ppu(ppuBus, interruptLines);

        mainBus.attachRegion(AddressRegion::WorkRam, memory);
        mainBus.attachRegion(AddressRegion::Ppu, ppu);
        mainBus.attachRegion(AddressRegion::ApuIo, cpu);

        // Start by setting up OAM in work RAM
        for (uint16 i = 0; i < 256; ++i) {
            mainBus.write(0x200 + i, i);
        }

        size_t cycles = clock.cpuCycles();

        // Initiate the DMA transfer at $0200
        mainBus.write(kOamDmaRegister, 0x02);

        for (uint16 i = 0; i < 256; ++i) {
            cpu.execute();
        }

        // DMA transfer should take 513 or 514 cycles
        CHECK_GE(clock.cpuCycles() - cycles, 513);
        CHECK_LE(clock.cpuCycles() - cycles, 514);

        // Ensure OAM data is written to PPU
        for (uint16 i = 0; i < 256; ++i) {
            ppu.write(kOamAddrRegister, i);
            CHECK_EQ(ppu.read(kOamDataRegister), i);
        }
    }
}
}
