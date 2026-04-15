#ifndef NESEMU_CARTRIDGE_H
#define NESEMU_CARTRIDGE_H

#include "emu_types.hpp"

#include <filesystem>

// # References
// https://www.nesdev.org/wiki/INES
// https://www.nesdev.org/wiki/NES_2.0
// https://www.nesdev.org/wiki/Mapper
// https://www.nesdev.org/wiki/NES_2.0_submappers
// https://www.nesdev.org/wiki/List_of_mappers
// https://www.nesdev.org/wiki/NROM

namespace NESEmu {

struct iNESCartridgeHeader {
    constexpr static uint32 kMagic = 0x1A53454E;

    uint32 magic;
    uint8  prgRomSize;
    uint8  chrRomSize;
    uint8  flag1;
    uint8  flag2;
    uint8  unused[8];
};

class Cartridge {
public:
    struct CartridgeInfo {
        uint8 prgRomSize;
        uint8 chrRomSize;
        uint8 mapper;
    };

    static Cartridge* loadFromFile(const std::filesystem::path& path);

private:
    explicit Cartridge(const CartridgeInfo& info);
    ~Cartridge() = default;

    static bool tryLoadiNESHeader(std::ifstream& file, CartridgeInfo& info);

    CartridgeInfo m_info;
};
}

#endif //NESEMU_CARTRIDGE_H
