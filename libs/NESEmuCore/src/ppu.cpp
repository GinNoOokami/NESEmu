#include "NESEmuCore/ppu.hpp"

#include "NESEmuCore/interrupt_lines.hpp"

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

void Ppu::execute(const int ppuCycles)
{
    for (int i = 0; i < ppuCycles; ++i) {
        m_dotCycle++;
        if (m_dotCycle >= kFrameScanlineWidth) {
            m_dotCycle = 0;
            advanceScanline();
        }
    }
}

uint8 Ppu::read(const uint16 address)
{
    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
        case PpuRegisters::kPpuMask:
            return m_dataLatch;
        case PpuRegisters::kPpuStatus:
            return readStatus();
        case PpuRegisters::kOamAddr:
            return m_oamAddr;
        case PpuRegisters::kOamData:
            return m_oam.raw[m_oamAddr];
        case PpuRegisters::kPpuScroll:
        case PpuRegisters::kPpuAddr:
        case PpuRegisters::kPpuData:
            break;
        default:
            assert(false && "Invalid PPU read address");
    }
    return m_dataLatch;
}

void Ppu::write(const uint16 address, const uint8 data)
{
    m_dataLatch = data;

    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
            m_ppuCtrl.value = data;
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
        case PpuRegisters::kPpuAddr:
        case PpuRegisters::kPpuData:
            break;
        default:
            assert(false && "Invalid PPU write address");
    }
}

uint8 Ppu::readStatus()
{
    uint8 status = m_ppuStatus.status() | (m_dataLatch & 0x1F);
    m_ppuStatus.vBlank(false);
    return status;
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
            break;
        case kFrameScanlineMax:
            m_scanline = 0;
            break;
        default:
            break;
    }
}
