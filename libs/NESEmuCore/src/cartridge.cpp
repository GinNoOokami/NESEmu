#include "NESEmuCore/cartridge.hpp"

#include <filesystem>
#include <fstream>

using namespace NESEmu;

Cartridge* Cartridge::loadFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Invalid ROM path: " + path.string());
    }

    CartridgeInfo info{};
    if (tryLoadiNESHeader(file, info)) {
        return new Cartridge(info);
    }

    // Not a supported ROM file
    return nullptr;
}

Cartridge::Cartridge(const CartridgeInfo& info)
    : m_info(info) {}

bool Cartridge::tryLoadiNESHeader(std::ifstream& file, CartridgeInfo& info)
{
    iNESCartridgeHeader header{};

    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (file.gcount() != static_cast<std::streamsize>(sizeof(header))) {
        throw std::runtime_error("Invalid ROM file");
    }

    if (header.magic != iNESCartridgeHeader::kMagic) {
        return false;
    }

    info.prgRomSize = header.prgRomSize * 16384;
    info.chrRomSize = header.chrRomSize * 8192;

    uint8 lo    = header.flag1 & 0xF0;
    uint8 hi    = header.flag2 & 0xF0;
    info.mapper = hi | (lo >> 4);

    return true;
}
