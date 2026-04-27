#include "NESEmuCore/ppu.hpp"

#include <cassert>

using namespace NESEmu;

void Ppu::startup() {}

void Ppu::reset() {}

void Ppu::execute() {}

uint8 Ppu::readBus(uint16 address) const
{
    switch (static_cast<PpuRegisters>(address & ADDRESS_MIRROR_MASK)) {
        case PpuRegisters::kPpuCtrl:
        case PpuRegisters::kPpuMask:
            return m_dataLatch;
        case PpuRegisters::kPpuStatus:
            break;
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
