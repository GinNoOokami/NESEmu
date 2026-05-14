#include "NESEmuCore/cartridge.hpp"

#include <filesystem>
#include <fstream>

using namespace NESEmu;

std::unique_ptr<Cartridge> Cartridge::createFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Invalid ROM path: " + path.string());
    }

    CartridgeInfo info{};
    info.path = path;

    if (tryLoadiNESHeader(file, info)) {
        return std::unique_ptr<Cartridge>(new Cartridge(info));
    }

    // Not a supported ROM file
    return nullptr;
}

std::unique_ptr<MapperNRom> Cartridge::loadMapper() const
{
    std::ifstream file(m_info.path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Invalid ROM path: " + m_info.path.string());
    }

    return std::unique_ptr<MapperNRom>(new MapperNRom(file, m_info.headerOffset, m_info.prgRomSizeBytes, m_info.chrRomSizeBytes));
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

    static_assert(sizeof(header) < 256, "Header size is too large");

    info.headerOffset    = sizeof(header);
    info.prgRomSizeBytes = header.prgRomSize * 16384;
    info.chrRomSizeBytes = header.chrRomSize * 8192;

    uint8 lo    = header.flag1 & 0xF0;
    uint8 hi    = header.flag2 & 0xF0;
    info.mapper = hi | (lo >> 4);

    return true;
}
