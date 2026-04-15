#include "NESEmuCore/cartridge.hpp"

#include <doctest.h>

using namespace NESEmu;

TEST_SUITE("Cartridge Tests") {

TEST_CASE("Invalid ROM path throws exception")
{
    CHECK_THROWS_WITH(Cartridge::loadFromFile("not/a/path/rom.nes"), doctest::Contains("Invalid ROM path"));
}

TEST_CASE("Too small ROM file throws exception")
{
    CHECK_THROWS_WITH(Cartridge::loadFromFile("data/rom/invalid_ines.nes"), doctest::Contains("Invalid ROM file"));
}

TEST_CASE("Invalid iNES file returns null")
{
    CHECK(Cartridge::loadFromFile("data/rom/invalid_rom.nes") == nullptr);
}

TEST_CASE("Valid iNES file returns Cartridge*")
{
    CHECK(Cartridge::loadFromFile("data/rom/official.nes"));
}

}
