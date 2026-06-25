#include "NESEmuCore/mapper_nrom.hpp"

#include <fstream>

using namespace NESEmu;

MapperNRom::MapperNRom(std::ifstream& file, const uint8 offset, const uint16 prgRomSize, const uint16 chrRomSize, const bool isHorizontalMirrored) :
    m_prgRomMask(prgRomSize == 0x4000 ? kPrgRom128AddressMask : kPrgRom256AddressMask),
    m_chrRomMask(chrRomSize > 0 ? chrRomSize - 1 : 0),
    m_isHorizontalMirrored(isHorizontalMirrored)
{
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(m_prgRom.data()), prgRomSize);
    file.read(reinterpret_cast<char*>(m_chrRom.data()), chrRomSize);
}
