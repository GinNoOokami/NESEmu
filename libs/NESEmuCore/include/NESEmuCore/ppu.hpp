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
        [[nodiscard]] bool addressIncrementMode() const { return value & 0b0000'0100; }

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
        [[nodiscard]] bool spriteZeroHit() const { return value & 0b0100'0000; }
        void               spriteZeroHit(const bool enable) { value = (value & ~0b0100'0000) | (enable << 6); }

        // (0: Off, 1: On)
        [[nodiscard]] bool vBlank() const { return value & 0b1000'0000; }
        void               vBlank(const bool enable) { value = (value & ~0b1000'0000) | (enable << 7); }

    private:
        uint8 value{};
    };

    struct PpuAddress {
        [[nodiscard]] uint8 coarseX() const { return value & kCoarseXBitsMask; }
        void                coarseX(const uint8 x) { value = (value & ~kCoarseXBitsMask) | x & 0x1F; }

        [[nodiscard]] uint8 coarseY() const { return (value & kCoarseYBitsMask) >> 5; }
        void                coarseY(const uint8 y) { value = (value & ~kCoarseYBitsMask) | (y << 5); }

        [[nodiscard]] uint8 nametableIndex() const { return (value & kNametableBitsMask) >> 10; }
        void                nametableIndex(const uint8 data) { value = (value & ~kNametableBitsMask) | (data << 10); }

        [[nodiscard]] uint8 fineY() const { return (value & kFineYBitsMask) >> 12; }
        void                fineY(const uint8 y) { value = (value & ~kFineYBitsMask) | (y << 12); }

        [[nodiscard]] uint8 msb() const { return value >> 8; }
        void                msb(const uint8 byte) { value = (value & 0x00FF) | (byte & 0x3F) << 8; }

        [[nodiscard]] uint8 lsb() const { return value & 0xFF; }
        void                lsb(const uint8 byte) { value = (value & 0xFF00) | byte; }

        void increment(const uint8 stride) { value = (value + stride) & 0x7FFF; }

        void incFineY()
        {
            if ((value & 0x7000) != 0x7000)
                value += 0x1000;
            else {
                value   &= ~0x7000;
                uint8 y = coarseY();
                if (y == 29) { (y = 0, value ^= 0x0800); } else { y += 1; }
                coarseY(y);
            }
        }

        void incCoarseX() { coarseX() == 31 ? (value ^= 0x400, value &= ~0x1F) : value += 1; }

        [[nodiscard]] uint16 horizontalBits() const { return value & kHorizontalBitsMask; }
        void                 horizontalBits(const PpuAddress& o) { value = value & ~kHorizontalBitsMask | o.horizontalBits(); }

        [[nodiscard]] uint16 verticalBits() const { return value & kVerticalBitsMask; }
        void                 verticalBits(const PpuAddress& o) { value = value & ~kVerticalBitsMask | o.verticalBits(); }

        [[nodiscard]] uint16 busAddress() const { return value & 0x3FFF; }
        [[nodiscard]] uint16 nametableAddress() const { return 0x2000 | (value & 0x0FFF); }

        [[nodiscard]] uint16 attributeTableAddress() const
        {
            return 0x23C0 | (value & 0x0C00) | ((value >> 4) & 0x38) | ((value >> 2) & 0x07);
        }

        friend bool operator==(const PpuAddress& lhs, const PpuAddress& rhs)
        {
            return lhs.value == rhs.value;
        }

        friend bool operator!=(const PpuAddress& lhs, const PpuAddress& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator==(const PpuAddress& lhs, const uint16 rhs)
        {
            return lhs.value == rhs;
        }

        friend bool operator!=(const PpuAddress& lhs, const uint16 rhs)
        {
            return !(lhs == rhs);
        }

    private:
        constexpr static uint16 kCoarseXBitsMask    = 0b0000'0000'0001'1111; // 0x001F
        constexpr static uint16 kCoarseYBitsMask    = 0b0000'0011'1110'0000;
        constexpr static uint16 kFineYBitsMask      = 0b0111'0000'0000'0000;
        constexpr static uint16 kHorizontalBitsMask = 0b0000'0100'0001'1111;
        constexpr static uint16 kVerticalBitsMask   = 0b0111'1011'1110'0000;
        constexpr static uint16 kNametableBitsMask  = 0b0000'1100'0000'0000;

        uint16 value{};
    };

    struct OamData {
        uint8 y;
        uint8 tile;
        uint8 attributes;
        uint8 x;
    };

    template <int Size>
    union Oam {
        std::array<uint8, Size * 4> raw;
        std::array<OamData, Size>   data;
    };

    struct InternalRegisters {
        PpuAddress v{};
        PpuAddress t{};
        uint8      x : 3{};
        uint8      w : 1{};
    };

public:
    struct PatternTable {
        class TilePattern {
        public:
            TilePattern(const uint8 bitPlaneLo, const uint8 bitPlaneHi) : m_bitPlaneLo(bitPlaneLo), m_bitPlaneHi(bitPlaneHi) {}

            [[nodiscard]] uint8 paletteIndex(const uint8 dot) const
            {
                const uint8 bit = 7 - dot;
                return ((m_bitPlaneHi >> bit & 1) << 1) | ((m_bitPlaneLo >> bit) & 1);
            }

        private:
            uint8 m_bitPlaneHi{};
            uint8 m_bitPlaneLo{};
        };

        [[nodiscard]] static uint16 address(const bool tableSelect, const bool plane, const uint8 tileIndex, const uint8 tileRow)
        {
            return (tableSelect << 12) | (tileIndex << 4) | (plane << 3) | (tileRow & 7);
        }

        [[nodiscard]] static TilePattern makeTile(const uint8 bitPlaneLo, const uint8 bitPlaneHi)
        {
            return { bitPlaneLo, bitPlaneHi };
        }
    };

private:
    /*
    struct SpriteBuffer {
        PatternTable::TilePattern patternSR;
        uint8                     x;
    };*/

public:
    static constexpr int kFrameScanlineWidth = 341;
    static constexpr int kFrameScanlineMax   = 262;
    static constexpr int kPpuCyclesPerFrame  = kFrameScanlineWidth * kFrameScanlineMax;

    using PrimaryOam  = Oam<64>;
    using InternalOam = Oam<8>;

    explicit Ppu(PpuBus& ppuBus, InterruptLines& interruptLines);

    void startup();
    void reset();

    void executeUntil(uint64 targetPpuCycles);

    [[nodiscard]] const PpuCtrl&           ppuCtrl() const { return m_ppuCtrl; }
    [[nodiscard]] const PpuMask&           ppuMask() const { return m_ppuMask; }
    [[nodiscard]] const PpuStatus&         ppuStatus() const { return m_ppuStatus; }
    [[nodiscard]] const FrameBuffer&       frameBuffer() const { return m_visibleFrameBuffer; }
    [[nodiscard]] const InternalRegisters& internalRegisters() const { return m_registers; }
    [[nodiscard]] const PrimaryOam&        oam() const { return m_oam; }
    [[nodiscard]] const InternalOam&       oamBuffer() const { return m_oamBuffer; }

    [[nodiscard]] uint8 onCpuRead(uint16 address);
    void                onCpuWrite(uint16 address, uint8 data);

    void updateVisibleFrameBuffer();

    [[nodiscard]] static uint8 selectPaletteFromAttribute(uint8 attribute, uint8 coarseX, uint8 coarseY)
    {
        const uint8 shiftOffset = ((coarseY & 0x2) | ((coarseX & 0x2) >> 1)) << 1;

        return (attribute >> shiftOffset) & 0x3;
    }

private:
    inline uint8 readStatus();
    inline void  setCtrlValue(uint8 data);
    inline void  writeScrollByte(uint8 data);
    inline void  writeAddressByte(uint8 data);
    inline void  writeDataByte(uint8 data);
    inline void  writePaletteData(uint8 index, uint8 data);
    inline uint8 readDataByte();
    inline uint8 readPaletteData(uint8 index);
    inline void  processSpriteEvaluation();

    void                updateScanline();
    void                advanceScanline();
    [[nodiscard]] uint8 evaluateSpritePaletteIndex(uint16 dot, bool isBgSolid, bool& priority);

private:
    using InternalFrameBuffer = std::array<std::array<PaletteIndex, kFrameScanlineWidth>, kFrameScanlineMax>;

    static constexpr std::array<uint8, 2> kStrideIncrements = { 1, 32 };

    PpuBus&         m_bus;
    InterruptLines& m_interruptLines;

    // MMIO registers
    PpuCtrl   m_ppuCtrl{};
    PpuMask   m_ppuMask{};
    PpuStatus m_ppuStatus{};
    uint8     m_oamAddr{};

    InternalRegisters     m_registers{};
    PrimaryOam            m_oam{};
    InternalOam           m_oamBuffer{};
    std::array<uint8, 32> m_paletteData{};

    uint8  m_dataBuffer{};
    uint8  m_busDataLatch{};
    uint16 m_dotCycle{};
    uint16 m_scanline{};
    uint64 m_cycles{};
    bool   m_canSpriteZeroTrigger{};

    uint8 m_nameTableByte{};
    uint8 m_patternTableHiByte{};
    uint8 m_patternTableLoByte{};
    uint8 m_attributeTableByte{};

    InternalFrameBuffer m_internalFrameBuffer{};
    FrameBuffer         m_visibleFrameBuffer{};
};
}

#endif //NESEMU_PPU_H
