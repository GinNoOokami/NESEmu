#ifndef NESEMU_PPU_H
#define NESEMU_PPU_H

#include "emu_types.hpp"

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
class PpuBus;
struct InterruptLines;

class Ppu {
    static constexpr int kFrameVisibleStart    = 0;
    static constexpr int kFramePostRenderStart = 240;
    static constexpr int kFrameVBlankStart     = 241;
    static constexpr int kFramePreRenderStart  = 261;

    static constexpr uint16 ADDRESS_MASK        = 0x3FFF;
    static constexpr uint8  ADDRESS_MIRROR_MASK = 0b0000'0111;

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
        [[nodiscard]] uint8 baseNametableAddress() const { return value & 0b0000'0011; }

        // (0: add 1, going across; 1: add 32, going down)
        [[nodiscard]] bool ramAddressIncrement() const { return value & 0b0000'0100; }

        // (0: $0000; 1: $1000; ignored in 8x16 mode)
        [[nodiscard]] bool spritePatternTableAddress() const { return value & 0b0000'1000; }

        // (0: $0000; 1: $1000)
        [[nodiscard]] bool backgroundPatternTableAddress() const { return value & 0b0001'0000; }

        // (0: 8x8 pixels; 1: 8x16 pixels)
        [[nodiscard]] bool spriteSize() const { return value & 0b0010'0000; }

        // (0: read backdrop from EXT pins; 1: output color on EXT pins)
        [[nodiscard]] bool masterSlaveSelect() const { return value & 0b0100'0000; }

        // (0: off, 1: on)
        [[nodiscard]] bool nmiEnable() const { return value & 0b1000'0000; }
    };

    struct PpuMask {
        uint8 value{};

        [[nodiscard]] bool isRenderingEnabled() const { return value & 0b0001'1000; }

        // (0: normal color, 1: greyscale)
        [[nodiscard]] uint8 greyscale() const { return value & 0b0000'0001; }

        // (0: Hide, 1: Show background in leftmost 8 pixels of screen)
        [[nodiscard]] uint8 backgroundColumnMask() const { return value & 0b0000'0010; }

        // (0: Hide, 1: Show sprites in leftmost 8 pixels of screen)
        [[nodiscard]] uint8 spriteColumnMask() const { return value & 0b0000'0100; }

        // (0: Disable, 1: Enable)
        [[nodiscard]] uint8 backgroundEnabled() const { return value & 0b0000'1000; }

        // (0: Disable, 1: Enable)
        [[nodiscard]] uint8 spriteEnabled() const { return value & 0b0001'0000; }

        // (0: Off, 1: On)
        [[nodiscard]] uint8 emphasizeRed() const { return value & 0b0010'0000; }

        // (0: Off, 1: On)
        [[nodiscard]] uint8 emphasizeGreen() const { return value & 0b0100'0000; }

        // (0: Off, 1: On)
        [[nodiscard]] uint8 emphasizeBlue() const { return value & 0b1000'0000; }
    };

    struct PpuStatus {
        [[nodiscard]] uint8 status() const { return value & 0b1110'0000; }

        // (0: Off, 1: On)
        [[nodiscard]] uint8 vBlank() const { return value & 0b1000'0000; }
        void                vBlank(const bool enable) { value = (value & ~0b1000'0000) | (enable << 7); }

    private:
        uint8 value{};
    };

    union Oam {
        struct OamData {
            uint8 y;
            uint8 tile;
            uint8 attributes;
            uint8 x;
        };

        std::array<uint8, 256>  raw;
        std::array<OamData, 64> data;
    };

public:
    static constexpr int kScreenDotWidth     = 256;
    static constexpr int kScreenDotHeight    = 240;
    static constexpr int kFrameScanlineWidth = 341;
    static constexpr int kFrameScanlineMax   = 262;
    static constexpr int kPpuCyclesPerFrame  = kFrameScanlineWidth * kFrameScanlineMax;

    using PaletteIndex = uint8;
    using FrameBuffer  = std::array<PaletteIndex, kScreenDotWidth * kScreenDotHeight>;

    explicit Ppu(PpuBus& ppuBus, InterruptLines& interruptLines);

    void startup();
    void reset();

    void executeUntil(uint64 targetPpuCycles);

    [[nodiscard]] const PpuCtrl&   ppuCtrl() const { return m_ppuCtrl; }
    [[nodiscard]] const PpuMask&   ppuMask() const { return m_ppuMask; }
    [[nodiscard]] const PpuStatus& ppuStatus() const { return m_ppuStatus; }

    [[nodiscard]] const FrameBuffer& frameBuffer() const { return m_frameBuffer; }

    [[nodiscard]] uint8 read(uint16 address);
    void                write(uint16 address, uint8 data);

private:
    inline uint8 readStatus();
    inline void  advanceScanline();

private:
    PpuBus&         m_ppuBus;
    InterruptLines& m_interruptLines;

    // MMIO registers
    PpuCtrl   m_ppuCtrl{};
    PpuMask   m_ppuMask{};
    PpuStatus m_ppuStatus{};
    uint8     m_oamAddr{};
    Oam       m_oam{};

    uint8  m_dataLatch{};
    uint16 m_dotCycle{};
    uint16 m_scanline{};
    uint64 m_cycles{};

    FrameBuffer m_frameBuffer{};
};
}

#endif //NESEMU_PPU_H
