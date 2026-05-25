#include "NESEmuCore/bus.hpp"

using namespace NESEmu;

void MainBus::write(const uint16 address, const uint8 data)
{
    auto& handler = getHandler(address);
    handler.write(handler.ctx, address, data);
}

uint8 MainBus::read(const uint16 address)
{
    auto& handler = getHandler(address);
    return handler.read(handler.ctx, address);
}

void PpuBus::write(const uint16 address, const uint8 data) const
{
    if (m_isCiRamEnabled && address >= 0x2000) {
        const auto ciramAddress = toLogicalCiRamAddress(address);
        m_ciram.write(ciramAddress, data);
    } else {
        m_cartridge.write(m_cartridge.ctx, address, data);
    }
}

uint8 PpuBus::read(const uint16 address) const
{
    if (m_isCiRamEnabled && address >= 0x2000) {
        const auto ciramAddress = toLogicalCiRamAddress(address);
        return m_ciram.read(ciramAddress);
    }
    return m_cartridge.read(m_cartridge.ctx, address);
}

uint16 PpuBus::toLogicalCiRamAddress(const uint16 address) const
{
    // Connect PPU A11 pin to CIRAM A10 pin if horizontally mirrored
    // The mask must be 0x400 for vertical and 0x800 for horizontal, hence the shift left on the mask first
    const auto horizontal   = static_cast<uint16>(m_isHorizontalMirrored);
    const auto ciramAddress = static_cast<uint16>((address & 0x03FF) | ((address & (0x0400 << horizontal)) >> horizontal));

    return ciramAddress;
}
