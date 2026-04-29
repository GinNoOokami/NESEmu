#include "NESEmuCore/ppu.hpp"

#include "NESEmuCore/interrupt_lines.hpp"

#include <cassert>

using namespace NESEmu;

Ppu::Ppu(PpuBus& ppuBus, InterruptLines& interruptLines)
    : m_ppuBus(ppuBus), m_interruptLines(interruptLines) {}

void Ppu::startup() {}

void Ppu::reset()
{
    m_pixelCount = 0;
    m_scanline   = 0;
}

void Ppu::execute(const int cycles)
{
    for (int i = 0; i < cycles; ++i) {
        m_pixelCount++;
        if (m_pixelCount >= kFrameScanlineWidth) {
            m_pixelCount = 0;
            advanceScanline();
        }
    }
}

uint8 Ppu::readBus(const uint16 address) const
{
    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
        case PpuRegisters::kPpuMask:
            return m_dataLatch;
        case PpuRegisters::kPpuStatus:
            return m_ppuStatus.status() | (m_dataLatch & 0x1F);
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

void Ppu::writeBus(const uint16 address, const uint8 data)
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

void Ppu::advanceScanline()
{
    // Check for scanline rollover
    if (m_scanline == kFramePreRenderStart) {
        m_scanline = -1;
    }

    m_scanline++;

    switch (m_scanline) {
        case kFrameVBlankStart:
            m_ppuStatus.vBlank(true);
            m_interruptLines.nmiActive = m_ppuCtrl.nmiEnable();
            break;
        case kFramePreRenderStart:
            m_ppuStatus.vBlank(false);
            break;
        default:
            break;
    }
}
