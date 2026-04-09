#include "cpu_step_tests.hpp"

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/cpu6502.hpp"
#include "NESEmuCore/memory.hpp"

#include <doctest.h>

using namespace NESEmu;

TEST_CASE("CPU Init State")
{
    FlatMemory memory;
    Bus        bus(memory);
    Cpu6502    cpu(bus);
    cpu.startup();

    CHECK_EQ(cpu.state().sp, 0xFD);
    CHECK(cpu.state().p);
    CHECK_EQ(cpu.state().a, 0);
    CHECK_EQ(cpu.state().x, 0);
    CHECK_EQ(cpu.state().y, 0);
}

std::string createTestName(const std::string& instructionDesc, const CpuStepTest& test)
{
    std::ostringstream oss;
    oss << "Test #" << test.key << " " << instructionDesc << ": " << test.name;
    return oss.str();
}

void runOpcodeStepTests(uint8_t opcode, const std::string& instructionDesc)
{
    std::vector<CpuStepTest> tests(10000);
    CpuStepTest::loadTestData(opcode, tests);

    for (auto& test : tests) {
        std::ostringstream oss;
        INFO("Executing test: ", createTestName(instructionDesc, test), "\ninitial_state=", test.initial_state);

        FlatMemory memory;
        Bus        bus(memory);
        Cpu6502    cpu(bus);

        test.initializeCpu(cpu, bus);
        auto initialCycles = cpu.cycles();

        cpu.execute();

        auto actual        = test.currentState(cpu, bus);
        auto elapsedCycles = cpu.cycles() - initialCycles;

        CHECK_EQ(test.final_state, actual);
        CHECK_EQ(test.cycles.size(), elapsedCycles);
    }
}

#define DEFINE_OPCODE_STEP_TEST(opcodeValue, descriptionText)       \
TEST_CASE(descriptionText)                                          \
{                                                                   \
    runOpcodeStepTests(opcodeValue, descriptionText);               \
}

DEFINE_OPCODE_STEP_TEST(0x06, "$06 ASL (zero page)")
DEFINE_OPCODE_STEP_TEST(0x0a, "$0a ASL (accumulator)")
DEFINE_OPCODE_STEP_TEST(0x0e, "$0e ASL (absolute)")
DEFINE_OPCODE_STEP_TEST(0x16, "$16 ASL (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x1e, "$1e ASL (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x21, "$21 AND (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x25, "$25 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x29, "$29 AND (immediate)")
DEFINE_OPCODE_STEP_TEST(0x2d, "$2d AND (absolute)")
DEFINE_OPCODE_STEP_TEST(0x31, "$31 AND (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x35, "$35 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x39, "$39 AND (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x3d, "$3d AND (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x61, "$61 ADC (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x69, "$69 ADC (immediate)")
DEFINE_OPCODE_STEP_TEST(0x6d, "$6d ADC (absolute)")
DEFINE_OPCODE_STEP_TEST(0x71, "$71 ADC (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x75, "$75 ADC (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0x79, "$79 ADC (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x7d, "$7d ADC (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0xc8, "$c8 INY (implied)")
DEFINE_OPCODE_STEP_TEST(0xe8, "$e8 INX (implied)")
DEFINE_OPCODE_STEP_TEST(0xea, "$ea NOP")

#undef DEFINE_OPCODE_STEP_TEST
