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
                    const uint8 color        = tileData.paletteIndex(i);
                    const uint8 paletteIndex = (color == 0) ? 0 : (paletteSelect << 2 | color);

                    m_internalFrameBuffer[m_scanline][m_registers.v.coarseX() * 8 + i] = m_paletteData[paletteIndex];
                }
                m_registers.v.incCoarseX();
            }
            // TODO: Temp code - remove when render fully implemented
            // Just output the nametable byte as a direct color palette lookup for now
            //m_internalFrameBuffer[m_scanline][m_dotCycle] = m_nameTableByte & 0x3F;

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
            updateVisibleFrameBuffer();
            break;
        case kFrameScanlineMax:
            m_scanline = 0;
            break;
        default:
            break;
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
