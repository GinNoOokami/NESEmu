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
DEFINE_OPCODE_STEP_TEST(0x10, "$10 BPL (relative)")
DEFINE_OPCODE_STEP_TEST(0x16, "$16 ASL (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x18, "$18 CLC (implied)")
DEFINE_OPCODE_STEP_TEST(0x1e, "$1e ASL (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x21, "$21 AND (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x24, "$24 BIT (zero page)")
DEFINE_OPCODE_STEP_TEST(0x25, "$25 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x29, "$29 AND (immediate)")
DEFINE_OPCODE_STEP_TEST(0x2c, "$2c BIT (absolute)")
DEFINE_OPCODE_STEP_TEST(0x2d, "$2d AND (absolute)")
DEFINE_OPCODE_STEP_TEST(0x30, "$30 BMI (relative)")
DEFINE_OPCODE_STEP_TEST(0x31, "$31 AND (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x35, "$35 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x39, "$39 AND (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x3d, "$3d AND (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x41, "$41 EOR (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x45, "$45 EOR (zero page)")
DEFINE_OPCODE_STEP_TEST(0x49, "$49 EOR (immediate)")
DEFINE_OPCODE_STEP_TEST(0x4d, "$4d EOR (absolute)")
DEFINE_OPCODE_STEP_TEST(0x50, "$50 BVC (relative)")
DEFINE_OPCODE_STEP_TEST(0x51, "$51 EOR (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x55, "$55 EOR (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x58, "$58 CLI (implied)")
DEFINE_OPCODE_STEP_TEST(0x59, "$59 EOR (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x5d, "$5d EOR (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x61, "$61 ADC (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x69, "$69 ADC (immediate)")
DEFINE_OPCODE_STEP_TEST(0x6d, "$6d ADC (absolute)")
DEFINE_OPCODE_STEP_TEST(0x70, "$70 BVS (relative)")
DEFINE_OPCODE_STEP_TEST(0x71, "$71 ADC (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x75, "$75 ADC (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0x79, "$79 ADC (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x7d, "$7d ADC (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0x88, "$88 DEY (implied)")
DEFINE_OPCODE_STEP_TEST(0x90, "$90 BCC (relative)")
DEFINE_OPCODE_STEP_TEST(0xb0, "$b0 BCS (relative)")
DEFINE_OPCODE_STEP_TEST(0xb8, "$b8 CLV (implied)")
DEFINE_OPCODE_STEP_TEST(0xc0, "$c0 CPY (immediate)")
DEFINE_OPCODE_STEP_TEST(0xc1, "$c1 CMP (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0xc4, "$c4 CPY (zero page)")
DEFINE_OPCODE_STEP_TEST(0xc5, "$c5 CMP (zero page)")
DEFINE_OPCODE_STEP_TEST(0xc6, "$c6 DEC (zero page)")
DEFINE_OPCODE_STEP_TEST(0xc8, "$c8 INY (implied)")
DEFINE_OPCODE_STEP_TEST(0xc9, "$c9 CMP (immediate)")
DEFINE_OPCODE_STEP_TEST(0xca, "$ca DEX (implied)")
DEFINE_OPCODE_STEP_TEST(0xcc, "$cc CPY (absolute)")
DEFINE_OPCODE_STEP_TEST(0xcd, "$cd CMP (absolute)")
DEFINE_OPCODE_STEP_TEST(0xce, "$ce DEC (absolute)")
DEFINE_OPCODE_STEP_TEST(0xd0, "$d0 BNE (relative)")
DEFINE_OPCODE_STEP_TEST(0xd1, "$d1 CMP (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0xd6, "$d6 DEC (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0xd8, "$d8 CLD (implied)")
DEFINE_OPCODE_STEP_TEST(0xd9, "$d9 CMP (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0xdd, "$dd CMP (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0xde, "$de DEC (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0xe0, "$e0 CPX (immediate)")
DEFINE_OPCODE_STEP_TEST(0xe4, "$e4 CPX (zero page)")
DEFINE_OPCODE_STEP_TEST(0xe8, "$e8 INX (implied)")
DEFINE_OPCODE_STEP_TEST(0xea, "$ea NOP (implied)")
DEFINE_OPCODE_STEP_TEST(0xec, "$ec CPX (absolute)")
DEFINE_OPCODE_STEP_TEST(0xf0, "$f0 BEQ (relative)")

#undef DEFINE_OPCODE_STEP_TEST
