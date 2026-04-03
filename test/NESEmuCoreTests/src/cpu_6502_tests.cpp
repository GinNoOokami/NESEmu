#include <doctest.h>
#include <utility>

#include "cpu_step_tests.hpp"
#include "NESEmuCore/cpu_6502.hpp"

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
    std::vector<CpuStepTest> tests(1000);
    CpuStepTest::from_json(0xE8, tests);

    for (const auto& test : tests) {
        SUBCASE("#" + test.key + " e8(INX) (implicit): " + test.name) {
            CPU_6502 cpu;

            // Execute instruction
            //cpu.execute();

            CPUTestState current_state;
            current_state.cpu = cpu.state();

            for (auto [ address, value ] : test.final_state.memory) {
                current_state.memory.emplace_back(address, cpu.readMemory(address));
            }

            CHECK_EQ(test.final_state, current_state);
        }
    }
}
