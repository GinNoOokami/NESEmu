#include <doctest.h>

#include "NESEmuCore/system.hpp"

using namespace NESEmu;

TEST_SUITE("System Tests") {
TEST_CASE("runFrame")
{
    SUBCASE("executes expected number of CPU cycles") {
        const auto cartridge = Cartridge::createFromFile("data/rom/run.6502.nes");

        System system;

        system.startup(*cartridge);
        system.runFrame();

        CHECK_GE(system.clock().cpuCycles(), 29780);
        CHECK_LE(system.clock().cpuCycles(), 29785);
    }

    SUBCASE("executes expected number of PPU cycles") {
        const auto cartridge = Cartridge::createFromFile("data/rom/run.6502.nes");

        System system;

        system.startup(*cartridge);
        system.runFrame();

        CHECK_GE(system.clock().ppuCycles(), 89341);
        CHECK_LE(system.clock().ppuCycles(), 89360);
    }
}
}
