#include "NESEmuCore/cartridge.hpp"
#include "NESEmuCore/memory.hpp"

#include <doctest.h>

using namespace NESEmu;

TEST_SUITE("Cartridge Tests") {

TEST_CASE("Invalid ROM path throws exception")
{
    CHECK_THROWS_WITH(Cartridge::createFromFile("not/a/path/rom.nes"), doctest::Contains("Invalid ROM path"));
}

TEST_CASE("Too small ROM file throws exception")
{
    CHECK_THROWS_WITH(Cartridge::createFromFile("data/rom/invalid_ines.nes"), doctest::Contains("Invalid ROM file"));
}

TEST_CASE("Invalid iNES file returns null")
{
    CHECK(Cartridge::createFromFile("data/rom/invalid_rom.nes") == nullptr);
}

TEST_CASE("Valid iNES file returns Cartridge*")
{
    CHECK(Cartridge::createFromFile("data/rom/official.nes"));
}

TEST_CASE("Load NROM cartridge initializes CPU PC to expected address")
{
    auto cartridge = Cartridge::createFromFile("data/rom/run.6502.nes");
    auto mapper    = cartridge->loadMapper();

    // Check that the reset vector address is in the correct location
    CHECK((mapper->read(0x3FFC) == 0x00));
    CHECK((mapper->read(0x3FFD) == 0x81));
}
}
