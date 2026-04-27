#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/cartridge.hpp"
#include "NESEmuCore/cpu6502.hpp"
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
    InternalRam memory;
    CpuBus      bus(memory);
    Cpu6502     cpu(bus);
    auto        cartridge = Cartridge::createFromFile("data/rom/run.6502.nes");
    auto        mapper    = cartridge->loadMapper();
    bus.mapper(mapper);

    cpu.startup();

    CHECK((cpu.state().pc == 0x8100));
}
}
