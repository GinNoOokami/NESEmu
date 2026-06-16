#include "NESEmuCore/ppu.hpp"

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/interrupt_lines.hpp"

#include <algorithm>
#include <cassert>

using namespace NESEmu;

Ppu::Ppu(PpuBus& ppuBus, InterruptLines& interruptLines)
    : m_bus(ppuBus), m_interruptLines(interruptLines) {}

void Ppu::startup() {}

void Ppu::reset()
{
    m_dotCycle = 0;
    m_scanline = 0;
}

void Ppu::executeUntil(const uint64 targetPpuCycles)
{
    while (m_cycles < targetPpuCycles) {
        if (m_ppuMask.isRenderingEnabled()) {
            updateScanline();
        }

        m_dotCycle++;
        m_cycles++;

        if (m_dotCycle >= kFrameScanlineWidth) {
            m_dotCycle = 0;
            advanceScanline();
        }
    }
}

uint8 Ppu::onCpuRead(const uint16 address)
{
    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
        case PpuRegisters::kPpuMask:
            return m_busDataLatch;
        case PpuRegisters::kPpuStatus:
            return readStatus();
        case PpuRegisters::kOamAddr:
            return m_oamAddr;
        case PpuRegisters::kOamData:
            return m_oam.raw[m_oamAddr];
        case PpuRegisters::kPpuScroll:
        case PpuRegisters::kPpuAddr:
            return m_busDataLatch;
        case PpuRegisters::kPpuData:
            return readDataByte();
        default:
            assert(false && "Invalid PPU read address");
    }
    return m_busDataLatch;
}

void Ppu::onCpuWrite(const uint16 address, const uint8 data)
{
    m_busDataLatch = data;

    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
            setCtrlValue(data);
            break;
        case PpuRegisters::kPpuMask:
            m_ppuMask.value = data;
            break;
        case PpuRegisters::kPpuStatus:
            break;
        case PpuRegisters::kOamAddr:
            m_oamAddr = data;
            break;
        case PpuRegisters::kOamData:
            m_oam.raw[m_oamAddr++] = data;
            break;
        case PpuRegisters::kPpuScroll:
            writeScrollByte(data);
            break;
        case PpuRegisters::kPpuAddr:
            writeAddressByte(data);
            break;
        case PpuRegisters::kPpuData:
            writeDataByte(data);
            break;
        default:
            assert(false && "Invalid PPU write address");
    }
}

uint8 Ppu::readStatus()
{
    uint8 status = m_ppuStatus.status() | (m_busDataLatch & 0x1F);
    m_ppuStatus.vBlank(false);
    m_registers.w = 0;
    return status;
}

void Ppu::setCtrlValue(const uint8 data)
{
    m_ppuCtrl.value = data;
    m_registers.t.nametableIndex(m_ppuCtrl.baseNametableAddress());
    m_interruptLines.nmiActive = m_ppuCtrl.nmiEnable() && m_ppuStatus.vBlank();
}

void Ppu::writeScrollByte(const uint8 data)
{
    if (m_registers.w) {
        m_registers.t.coarseY(data >> 3);
        m_registers.t.fineY(data & 0x7);
    } else {
        m_registers.t.coarseX(data >> 3);
        m_registers.x = data & 0x7;
    }
    m_registers.w = !m_registers.w;
}

void Ppu::writeAddressByte(const uint8 data)
{
    if (m_registers.w) {
        m_registers.t.lsb(data);
        m_registers.v = m_registers.t;
    } else {
        m_registers.t.msb(data);
    }
    m_registers.w = !m_registers.w;
}

void Ppu::writeDataByte(const uint8 data)
{
    if ((m_registers.v.busAddress() & 0x3F00) == 0x3F00) {
        const uint8 index = m_registers.v.busAddress() & 0x001F;
        writePaletteData(index, data);
    } else {
        m_bus.write(m_registers.v.busAddress(), data);
    }
    m_registers.v.increment(kStrideIncrements[m_ppuCtrl.addressIncrementMode()]);
}

void Ppu::writePaletteData(uint8 index, const uint8 data)
{
    if ((index & 3) == 0) { index &= ~0x10; }
    m_paletteData[index] = data;
}

uint8 Ppu::readDataByte()
{
    uint8 value;

    if ((m_registers.v.busAddress() & 0x3F00) == 0x3F00) {
        const uint8 index = m_registers.v.busAddress() & 0x001F;
        value             = readPaletteData(index);
    } else {
        value = m_dataBuffer;
    }

    m_dataBuffer = m_bus.read(m_registers.v.busAddress());
    m_registers.v.increment(kStrideIncrements[m_ppuCtrl.addressIncrementMode()]);

    return value;
}

uint8 Ppu::readPaletteData(uint8 index)
{
    if ((index & 3) == 0) { index &= ~0x10; }
    return m_paletteData[index];
}

void Ppu::updateScanline()
{
    if (m_scanline < 240 || m_scanline == kFramePreRenderStart) {
        if (m_dotCycle > 0 && m_dotCycle < 257) {
            // This isn't cycle accurate yet; we wait until the end of the tile and draw it all at once
            if ((m_dotCycle & 7) == 0) {
                // Look up the nametable entry
                m_nameTableByte = m_bus.read(m_registers.v.nametableAddress());

                // Look up attribute table entry
                m_attributeTableByte = m_bus.read(m_registers.v.attributeTableAddress());

                // Look up pattern lo and hi bytes
                const auto patternAddressLo = PatternTable::address(m_ppuCtrl.backgroundPatternTableAddress(), false, m_nameTableByte, m_registers.v.fineY());
                const auto patternAddressHi = PatternTable::address(m_ppuCtrl.backgroundPatternTableAddress(), true, m_nameTableByte, m_registers.v.fineY());

                m_patternTableLoByte = m_bus.read(patternAddressLo);
                m_patternTableHiByte = m_bus.read(patternAddressHi);

                // Fetch the current palette table based on the attribute table entry
                const auto paletteSelect = selectPaletteFromAttribute(m_attributeTableByte, m_registers.v.coarseX(), m_registers.v.coarseY());

                // for each pixel in the tile:
                // * combine the pattern bits to form the palette index
                // * look up the final palette index
                // * insert value into the buffer array at the current scanline dot
                const auto tileData = PatternTable::makeTile(m_patternTableLoByte, m_patternTableHiByte);
                for (uint8 i = 0; i < 8; ++i) {
                    const uint16 dot          = m_dotCycle - 8 + i;
                    uint8        bgColor      = 0;
                    uint8        paletteIndex = 0;

                    if (m_ppuMask.backgroundEnabled()) {
                        bgColor      = tileData.paletteIndex(i);
                        paletteIndex = (bgColor == 0) ? 0 : (paletteSelect << 2 | bgColor);
                    }

                    if (m_ppuMask.spriteEnabled()) {
                        bool  priority           = false;
                        uint8 spritePaletteIndex = evaluateSpritePaletteIndex(dot, bgColor > 0, priority);
                        bool  bgPriority         = priority && bgColor > 0;
                        bool  drawSpriteDot      = (!priority || !bgColor) && !bgPriority;
                        if (drawSpriteDot && spritePaletteIndex & 0x03) {
                            paletteIndex = spritePaletteIndex & 0x1F;
                        }
                    }

                    m_internalFrameBuffer[m_scanline][dot] = m_paletteData[paletteIndex];
                }
                m_registers.v.incCoarseX();
            }

            if (m_dotCycle == 256) {
                m_registers.v.incFineY();
            }
        }

        if (m_dotCycle == 257) {
            m_registers.v.horizontalBits(m_registers.t);
        }
        if (m_scanline == kFramePreRenderStart) {
            if (m_dotCycle >= 280 && m_dotCycle <= 304) {
                m_registers.v.verticalBits(m_registers.t);
            }
        }
    }
}

void Ppu::advanceScanline()
{
    switch (++m_scanline) {
        case kFrameVBlankStart:
            m_ppuStatus.vBlank(true);
            m_interruptLines.nmiActive = m_ppuCtrl.nmiEnable();
            break;
        case kFramePreRenderStart:
            m_ppuStatus.vBlank(false);
            m_ppuStatus.spriteZeroHit(false);
            updateVisibleFrameBuffer();
            break;
        case kFrameScanlineMax:
            m_scanline = 0;
            break;
        default:
            break;
    }
    processSpriteEvaluation();
}

uint8 Ppu::evaluateSpritePaletteIndex(const uint16 dot, const bool isBgSolid, bool& priority)
{
    uint8 spriteColor        = 0;
    uint8 spritePaletteIndex = 0;

    for (int j = 7; j >= 0; --j) {
        const auto& [y, tile, attributes, x] = m_oamBuffer.data[j];
        if (y != 0xFF && dot >= x && dot < x + 8) {
            uint8 row = (m_scanline - y - 1) & 0x7;
            uint8 col = (dot - x) & 0x7;

            if (attributes & 0x80) {
                row = 7 - row;
            }

            if (attributes & 0x40) {
                col = 7 - col;
            }

            const auto spritePatternAddressLo = PatternTable::address(m_ppuCtrl.spritePatternTableAddress(), false, tile, row);
            const auto spritePatternAddressHi = PatternTable::address(m_ppuCtrl.spritePatternTableAddress(), true, tile, row);

            const auto spritePattern = PatternTable::makeTile(m_bus.read(spritePatternAddressLo), m_bus.read(spritePatternAddressHi));

            // Only set the sprite color if it's not transparent
            if (const auto color = spritePattern.paletteIndex(col)) {
                spriteColor        = color;
                spritePaletteIndex = attributes & 0x03;
                priority           = attributes & 0x20;
            }

            if (j == 0 && m_canSpriteZeroTrigger && isBgSolid && spriteColor > 0 && dot != 255) {
                m_ppuStatus.spriteZeroHit(true);
            }
        }
    }

    return 0x10 | (spritePaletteIndex << 2 | spriteColor);
}

void Ppu::processSpriteEvaluation()
{
    if (m_scanline < kFramePostRenderStart) {
        // Clear internal OAM to 0xFF each scanline
        for (int i = 0; i < 32; ++i) {
            m_oamBuffer.raw[i] = 0xFF;
        }

        m_canSpriteZeroTrigger = false;

        int count = 0;
        for (int i = 0; i < 64; ++i) {
            const auto& spriteData = m_oam.data[i];
            if (spriteData.y != 0xFF && m_scanline > spriteData.y && m_scanline <= spriteData.y + 8) {
                if (count == 8) {
                    // TODO: Set overflow flag
                    break;
                }

                m_oamBuffer.data[count++] = spriteData;
                if (i == 0) { m_canSpriteZeroTrigger = true; }
            }
        }
    }
}

void Ppu::updateVisibleFrameBuffer()
{
    for (int y = 0; y < kScreenDotHeight; ++y) {
        std::copy_n(
            m_internalFrameBuffer[y].begin(),
            kScreenDotWidth,
            m_visibleFrameBuffer.begin() + y * kScreenDotWidth
            );
    }
}
