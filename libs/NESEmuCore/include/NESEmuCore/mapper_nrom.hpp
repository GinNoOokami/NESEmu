#ifndef NESEMU_MAPPER_NROM_HPP
#define NESEMU_MAPPER_NROM_HPP

#include "emu_types.hpp"

#include <array>
#include <iosfwd>

namespace NESEmu {
class MapperNRom {
    static constexpr uint16 kPrgRom128AddressMask = 0x3FFF;
    static constexpr uint16 kPrgRom256AddressMask = 0x7FFF;

public:
    MapperNRom(std::ifstream& file, uint8 offset, uint16 prgRomSize, uint16 chrRomSize, bool isHorizontalMirrored);

    [[nodiscard]] uint8 onCpuRead(uint16 address) { return m_prgRom[address & m_prgRomMask]; }
    void                onCpuWrite(const uint16 address, const uint8 data) { m_prgRom[address] = data; }

    [[nodiscard]] uint8       onPpuRead(const uint16 address) { return m_chrRom[address & m_chrRomMask]; }
    void                      onPpuWrite(const uint16 address, const uint8 data) { m_chrRom[address & m_chrRomMask] = data; }
    [[nodiscard]] static bool isCiRamEnabled() { return true; }
    [[nodiscard]] bool        isHorizontalMirrored() const { return m_isHorizontalMirrored; }

private:
    uint16 m_prgRomMask{};
    uint16 m_chrRomMask{};

    bool m_isHorizontalMirrored{};

    std::array<uint8, 0x8000> m_prgRom{};
    std::array<uint8, 0x2000> m_chrRom{};
};
}

#endif //NESEMU_MAPPER_NROM_HPP
