#include <doctest.h>
#include <utility>

#include "cpu_step_tests.hpp"
#include "NESEmuCore/cpu_6502.hpp"
#include "NESEmuCore/debug_format.hpp"

using namespace NESEmu;

TEST_CASE("CPU Init State") {
    CPU_6502 cpu;
    cpu.startup();

    CHECK_EQ(cpu.state().sp, 0xFD);
    CHECK(cpu.state().p);
    CHECK_EQ(cpu.state().a, 0);
    CHECK_EQ(cpu.state().x, 0);
    CHECK_EQ(cpu.state().y, 0);
}

TEST_CASE("CPU Step Tests") {
    auto test = CpuStepTest::from_json(0xE8);

    SUBCASE(test.name) {
        CPU_6502 cpu;

        CHECK_EQ(test.final_state.cpu, cpu.state());

        for (auto [ address, value ] : test.final_state.memory) {
            auto expectedAddressValue = to_register(address) + "=" + doctest::toString(value);
            auto actualAddressValue = to_register(address) + "=" + doctest::toString(cpu.readMemory(address));

            CHECK_EQ(expectedAddressValue, actualAddressValue);
        }
    }
}
