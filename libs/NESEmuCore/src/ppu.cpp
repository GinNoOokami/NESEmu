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
            return m_dataLatch;
        case PpuRegisters::kPpuMask:
        case PpuRegisters::kPpuStatus:
        case PpuRegisters::kOamAddr:
        case PpuRegisters::kOamData:
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
        case PpuRegisters::kPpuStatus:
        case PpuRegisters::kOamAddr:
        case PpuRegisters::kOamData:
        case PpuRegisters::kPpuScroll:
        case PpuRegisters::kPpuAddr:
        case PpuRegisters::kPpuData:
            break;
        default:
            assert(false && "Invalid PPU write address");
    }
}
