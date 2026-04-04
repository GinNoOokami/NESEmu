#include "cpu_step_tests.hpp"
#include "opcode_descriptors.hpp"

#include "NESEmuCore/cpu_6502.hpp"

#include <doctest.h>

using namespace NESEmu;

std::string createTestName(const std::string& instructionDesc, const CpuStepTest& test) {
    std::ostringstream oss;
    oss << "Test #" << test.key << " " << instructionDesc << ": " << test.name;
    return oss.str();
}

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
    std::vector<CpuStepTest> tests(10000);

    for (const auto& [opcode, instructionDesc] : opcodeDescriptionList) {
        MESSAGE("Executing opcode tests: ", instructionDesc);
        CpuStepTest::from_json(opcode, tests);

        for (const auto& test : tests) {
            INFO("Executing test: ", createTestName(instructionDesc, test));

            CPU_6502 cpu;
            test.initializeCpu(cpu);

            // Execute instruction
            cpu.execute();

            auto actual = test.currentState(cpu);
            CHECK_EQ(test.final_state, actual);
        }
    }
}
