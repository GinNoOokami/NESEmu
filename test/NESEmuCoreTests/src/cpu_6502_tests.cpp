#include "cpu_step_tests.hpp"
#include "opcode_descriptors.hpp"

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/cpu_6502.hpp"
#include "NESEmuCore/memory.hpp"

#include <doctest.h>

using namespace NESEmu;

std::string createTestName(const std::string& instructionDesc, const CpuStepTest& test) {
    std::ostringstream oss;
    oss << "Test #" << test.key << " " << instructionDesc << ": " << test.name;
    return oss.str();
}

TEST_CASE("CPU Init State") {
    FlatMemory memory;
    Bus bus(&memory);
    CPU_6502 cpu(&bus);
    cpu.startup();

    auto _ = memory.read(0x00);

    CHECK_EQ(cpu.state().sp, 0xFD);
    CHECK(cpu.state().p);
    CHECK_EQ(cpu.state().a, 0);
    CHECK_EQ(cpu.state().x, 0);
    CHECK_EQ(cpu.state().y, 0);
}

TEST_CASE("CPU Step Tests") {
    std::vector<CpuStepTest> tests(10000);

    for (const auto& [opcode, instructionDesc] : opcodeDescriptionList) {
        SUBCASE(instructionDesc) {
            CpuStepTest::from_json(opcode, tests);

            for (const auto& test : tests) {
                INFO("Executing test: ", createTestName(instructionDesc, test));

                // Setup initial state
                FlatMemory memory;
                Bus bus(&memory);
                CPU_6502 cpu(&bus);
                test.initializeCpu(cpu);
                auto initialCycles = cpu.cycles();

                // Execute instruction
                cpu.execute();

                // Test final state
                auto actual = test.currentState(cpu);
                auto elapsedCycles = cpu.cycles() - initialCycles;
                CHECK_EQ(test.final_state, actual);
                CHECK_EQ(test.cycles.size(), elapsedCycles);
            }
        }
    }
}
