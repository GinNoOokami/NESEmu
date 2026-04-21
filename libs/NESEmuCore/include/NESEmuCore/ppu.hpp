#ifndef NESEMU_PPU_H
#define NESEMU_PPU_H

#include "emu_types.hpp"
#include "bus_mappable.hpp"

#include <array>

/*

Name        Address	Type	Notes
PPUCTRL	    $2000	W	NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), sprite tile select (S), increment mode (I), nametable select / X and Y scroll bit 8 (NN)
PPUMASK	    $2001	W	color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G)
PPUSTATUS   $2002	R	vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006
OAMADDR	    $2003	W	OAM read/write address
OAMDATA	    $2004	RW	OAM data read/write
PPUSCROLL   $2005	Wx2	X and Y scroll bits 7-0 (two writes: X scroll, then Y scroll)
PPUADDR	    $2006	Wx2	VRAM address (two writes: most significant byte, then least significant byte)
PPUDATA	    $2007	RW	VRAM data read/write
OAMDMA	    $4014	W	OAM DMA high address

Register types:

R - Readable
W - Writeable
x2 - Internal 2-byte state accessed by two 1-byte accesses
*/

namespace NESEmu {
class Ppu : public BusMappable<Ppu> {
    template <typename>
    friend class BusMappable;

    static constexpr uint16 ADDRESS_MASK        = 0x3FFF;
    static constexpr uint8  ADDRESS_MIRROR_MASK = 0b00000111;

    enum class PpuRegisters : uint8 {
        kPpuCtrl,
        kPpuMask,
        kPpuStatus,
        kOamAddr,
        kOamData,
        kPpuScroll,
        kPpuAddr,
        kPpuData,
    };

    struct PpuCtrl {
        uint8 value{};

        // (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
        [[nodiscard]] uint8 baseNametableAddress() const { return value & 0b00000011; }

        // (0: add 1, going across; 1: add 32, going down)
        [[nodiscard]] bool ramAddressIncrement() const { return value & 0b00000100; }

        // (0: $0000; 1: $1000; ignored in 8x16 mode)
        [[nodiscard]] bool spritePatternTableAddress() const { return value & 0b00001000; }

        // (0: $0000; 1: $1000)
        [[nodiscard]] bool backgroundPatternTableAddress() const { return value & 0b00010000; }

        // (0: 8x8 pixels; 1: 8x16 pixels)
        [[nodiscard]] bool spriteSize() const { return value & 0b00100000; }

        // (0: read backdrop from EXT pins; 1: output color on EXT pins)
        [[nodiscard]] bool masterSlaveSelect() const { return value & 0b01000000; }

        // (0: off, 1: on)
        [[nodiscard]] bool nmiEnable() const { return value & 0b10000000; }
    };

    struct OamData {
        uint8 y;
        uint8 tile;
        uint8 attributes;
        uint8 x;
    };

public:
    void startup();
    void reset();
    void execute();
    
    [[nodiscard]] const PpuCtrl& ppuCtrl() const { return m_ppuCtrl; }

protected:
    [[nodiscard]] uint8 readBus(uint16 address) const;
    void                writeBus(uint16 address, uint8 data);

private:
    std::array<uint8, 0x4000> m_memory{};
    std::array<OamData, 256>  m_oam{};
    uint8                     m_dataLatch{};

    PpuCtrl m_ppuCtrl{};
};
}

#endif //NESEMU_PPU_H
