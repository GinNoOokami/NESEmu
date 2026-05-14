#ifndef NESEMU_MAPPER_NROM_HPP
#define NESEMU_MAPPER_NROM_HPP

#include "emu_types.hpp"

#include <array>
#include <iosfwd>

namespace NESEmu {
class MapperNRom {
    static constexpr uint16 ROM128_ADDRESS_MASK = 0x3FFF;
    static constexpr uint16 ROM256_ADDRESS_MASK = 0x7FFF;

public:
    MapperNRom(std::ifstream& file, uint8 offset, uint16 prgRomSize, uint16 chrRomSize);

    [[nodiscard]] uint8 read(uint16 address) { return m_prgRom[address & m_prgRomMask]; }
    void                write(const uint16 address, const uint8 data) { m_prgRom[address] = data; }

private:
    uint16 m_prgRomMask;

    std::array<uint8, 0x8000> m_prgRom{};
    std::array<uint8, 0x2000> m_chrRom{};
};
}

#endif //NESEMU_MAPPER_NROM_HPP
