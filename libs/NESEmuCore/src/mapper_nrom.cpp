#include "NESEmuCore/mapper_nrom.hpp"

#include <fstream>

NESEmu::MapperNRom::MapperNRom(std::ifstream& file, const uint8 offset, const uint16 prgRomSize, const uint16 chrRomSize) :
    m_prgRomMask(prgRomSize == 0x4000 ? ROM128_ADDRESS_MASK : ROM256_ADDRESS_MASK)
{
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(m_prgRom.data()), prgRomSize);
    file.read(reinterpret_cast<char*>(m_chrRom.data()), chrRomSize);
}

NESEmu::uint8 NESEmu::MapperNRom::readBus(const uint16 address) const
{
    return m_prgRom[address & m_prgRomMask];
}
