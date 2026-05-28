#include "NESEmuCore/ppu.hpp"

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/interrupt_lines.hpp"

#include <algorithm>
#include <cassert>

using namespace NESEmu;

Ppu::Ppu(PpuBus& ppuBus, InterruptLines& interruptLines)
    : m_ppuBus(ppuBus), m_interruptLines(interruptLines) {}

void Ppu::startup() {}

void Ppu::reset()
{
    m_dotCycle = 0;
    m_scanline = 0;
}

void Ppu::executeUntil(const uint64 targetPpuCycles)
{
    while (m_cycles < targetPpuCycles) {
        // TODO: Temp code to get something on screen - select a random palette index based on current scanline
        m_internalFrameBuffer[m_scanline][m_dotCycle] = m_scanline & 0x3F;

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

void Ppu::setCtrlValue(uint8 data)
{
    m_ppuCtrl.value = data;
    m_registers.t.nametableIndex(m_ppuCtrl.baseNametableAddress());
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
        const uint8 index    = m_registers.v.busAddress() & 0x001F;
        m_paletteData[index] = data;
    } else {
        m_ppuBus.write(m_registers.v.busAddress(), data);
    }
    m_registers.v.increment(kStrideIncrements[m_ppuCtrl.addressIncrementMode()]);
}

uint8 Ppu::readDataByte()
{
    uint8 value;

    if ((m_registers.v.busAddress() & 0x3F00) == 0x3F00) {
        const uint8 index = m_registers.v.busAddress() & 0x001F;
        value             = m_paletteData[index];
    } else {
        value = m_dataBuffer;
    }

    m_dataBuffer = m_ppuBus.read(m_registers.v.busAddress());
    m_registers.v.increment(kStrideIncrements[m_ppuCtrl.addressIncrementMode()]);

    return value;
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
