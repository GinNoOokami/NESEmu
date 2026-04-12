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

DEFINE_OPCODE_STEP_TEST(0x01, "$01 ORA (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x05, "$05 ORA (zero page)")
DEFINE_OPCODE_STEP_TEST(0x06, "$06 ASL (zero page)")
DEFINE_OPCODE_STEP_TEST(0x08, "$08 PHP (implied)")
DEFINE_OPCODE_STEP_TEST(0x09, "$09 ORA (immediate)")
DEFINE_OPCODE_STEP_TEST(0x0a, "$0a ASL (accumulator)")
DEFINE_OPCODE_STEP_TEST(0x0d, "$0d ORA (absolute)")
DEFINE_OPCODE_STEP_TEST(0x0e, "$0e ASL (absolute)")
DEFINE_OPCODE_STEP_TEST(0x10, "$10 BPL (relative)")
DEFINE_OPCODE_STEP_TEST(0x11, "$11 ORA (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x15, "$15 ORA (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x16, "$16 ASL (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x18, "$18 CLC (implied)")
DEFINE_OPCODE_STEP_TEST(0x19, "$19 ORA (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x1d, "$1d ORA (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x1e, "$1e ASL (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x20, "$20 JSR (absolute)")
DEFINE_OPCODE_STEP_TEST(0x21, "$21 AND (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x24, "$24 BIT (zero page)")
DEFINE_OPCODE_STEP_TEST(0x25, "$25 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x26, "$26 ROL (zero page)")
DEFINE_OPCODE_STEP_TEST(0x28, "$28 PLP (implied)")
DEFINE_OPCODE_STEP_TEST(0x29, "$29 AND (immediate)")
DEFINE_OPCODE_STEP_TEST(0x2a, "$2a ROL (accumulator)")
DEFINE_OPCODE_STEP_TEST(0x2c, "$2c BIT (absolute)")
DEFINE_OPCODE_STEP_TEST(0x2d, "$2d AND (absolute)")
DEFINE_OPCODE_STEP_TEST(0x2e, "$2e ROL (absolute)")
DEFINE_OPCODE_STEP_TEST(0x30, "$30 BMI (relative)")
DEFINE_OPCODE_STEP_TEST(0x31, "$31 AND (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x35, "$35 AND (zero page)")
DEFINE_OPCODE_STEP_TEST(0x36, "$36 ROL (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x38, "$38 SEC (implied)")
DEFINE_OPCODE_STEP_TEST(0x39, "$39 AND (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x3d, "$3d AND (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x3e, "$3e ROL (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x41, "$41 EOR (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x45, "$45 EOR (zero page)")
DEFINE_OPCODE_STEP_TEST(0x46, "$46 LSR (zero page)")
DEFINE_OPCODE_STEP_TEST(0x48, "$48 PHA (implied)")
DEFINE_OPCODE_STEP_TEST(0x49, "$49 EOR (immediate)")
DEFINE_OPCODE_STEP_TEST(0x4a, "$4a LSR (accumulator)")
DEFINE_OPCODE_STEP_TEST(0x4c, "$4c JMP (absolute)")
DEFINE_OPCODE_STEP_TEST(0x4d, "$4d EOR (absolute)")
DEFINE_OPCODE_STEP_TEST(0x4e, "$4e LSR (absolute)")
DEFINE_OPCODE_STEP_TEST(0x50, "$50 BVC (relative)")
DEFINE_OPCODE_STEP_TEST(0x51, "$51 EOR (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x55, "$55 EOR (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x56, "$56 LSR (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x58, "$58 CLI (implied)")
DEFINE_OPCODE_STEP_TEST(0x59, "$59 EOR (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x5d, "$5d EOR (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x5e, "$5e LSR (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0x60, "$60 RTS (implied)")
DEFINE_OPCODE_STEP_TEST(0x61, "$61 ADC (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x66, "$66 ROR (zero page)")
DEFINE_OPCODE_STEP_TEST(0x68, "$68 PLA (implied)")
DEFINE_OPCODE_STEP_TEST(0x69, "$69 ADC (immediate)")
DEFINE_OPCODE_STEP_TEST(0x6a, "$6a ROR (accumulator)")
DEFINE_OPCODE_STEP_TEST(0x6c, "$6c JMP (indirect)")
DEFINE_OPCODE_STEP_TEST(0x6d, "$6d ADC (absolute)")
DEFINE_OPCODE_STEP_TEST(0x6e, "$6e ROR (absolute)")
DEFINE_OPCODE_STEP_TEST(0x70, "$70 BVS (relative)")
DEFINE_OPCODE_STEP_TEST(0x71, "$71 ADC (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x75, "$75 ADC (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0x76, "$76 ROR (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0x78, "$78 SEI (implied)")
DEFINE_OPCODE_STEP_TEST(0x79, "$79 ADC (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x7d, "$7d ADC (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0x7e, "$7e ROR (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0x81, "$81 STA (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0x84, "$84 STY (zero page)")
DEFINE_OPCODE_STEP_TEST(0x85, "$85 STA (zero page)")
DEFINE_OPCODE_STEP_TEST(0x86, "$86 STX (zero page)")
DEFINE_OPCODE_STEP_TEST(0x88, "$88 DEY (implied)")
DEFINE_OPCODE_STEP_TEST(0x8a, "$8a TXA (implied)")
DEFINE_OPCODE_STEP_TEST(0x8c, "$8c STY (absolute)")
DEFINE_OPCODE_STEP_TEST(0x8d, "$8d STA (absolute)")
DEFINE_OPCODE_STEP_TEST(0x8e, "$8e STX (absolute)")
DEFINE_OPCODE_STEP_TEST(0x90, "$90 BCC (relative)")
DEFINE_OPCODE_STEP_TEST(0x91, "$91 STA (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0x94, "$94 STY (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0x95, "$95 STA (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0x96, "$96 STX (zero page,Y)")
DEFINE_OPCODE_STEP_TEST(0x98, "$98 TYA (implied)")
DEFINE_OPCODE_STEP_TEST(0x99, "$99 STA (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0x9a, "$9a TXS (implied)")
DEFINE_OPCODE_STEP_TEST(0x9d, "$9d STA (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0xa0, "$a0 LDY (immediate)")
DEFINE_OPCODE_STEP_TEST(0xa1, "$a1 LDA (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0xa2, "$a2 LDX (immediate)")
DEFINE_OPCODE_STEP_TEST(0xa4, "$a4 LDY (zero page)")
DEFINE_OPCODE_STEP_TEST(0xa5, "$a5 LDA (zero page)")
DEFINE_OPCODE_STEP_TEST(0xa6, "$a6 LDX (zero page)")
DEFINE_OPCODE_STEP_TEST(0xa8, "$a8 TAY (implied)")
DEFINE_OPCODE_STEP_TEST(0xa9, "$a9 LDA (immediate)")
DEFINE_OPCODE_STEP_TEST(0xaa, "$aa TAX (implied)")
DEFINE_OPCODE_STEP_TEST(0xac, "$ac LDY (absolute)")
DEFINE_OPCODE_STEP_TEST(0xad, "$ad LDA (absolute)")
DEFINE_OPCODE_STEP_TEST(0xae, "$ae LDX (absolute)")
DEFINE_OPCODE_STEP_TEST(0xb0, "$b0 BCS (relative)")
DEFINE_OPCODE_STEP_TEST(0xb1, "$b1 LDA (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0xb4, "$b4 LDY (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0xb5, "$b5 LDA (zero page,x)")
DEFINE_OPCODE_STEP_TEST(0xb6, "$b6 LDX (zero page,y)")
DEFINE_OPCODE_STEP_TEST(0xb8, "$b8 CLV (implied)")
DEFINE_OPCODE_STEP_TEST(0xb9, "$b9 LDA (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0xba, "$ba TSX (implied)")
DEFINE_OPCODE_STEP_TEST(0xbc, "$bc LDY (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0xbd, "$bd LDA (absolute,x)")
DEFINE_OPCODE_STEP_TEST(0xbe, "$be LDX (absolute,y)")
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
DEFINE_OPCODE_STEP_TEST(0xe1, "$e1 SBC (indirect,X)")
DEFINE_OPCODE_STEP_TEST(0xe4, "$e4 CPX (zero page)")
DEFINE_OPCODE_STEP_TEST(0xe5, "$e5 CPX (zero page)")
DEFINE_OPCODE_STEP_TEST(0xe6, "$e6 INC (zero page)")
DEFINE_OPCODE_STEP_TEST(0xe8, "$e8 INX (implied)")
DEFINE_OPCODE_STEP_TEST(0xe9, "$e9 SBC (immediate)")
DEFINE_OPCODE_STEP_TEST(0xea, "$ea NOP (implied)")
DEFINE_OPCODE_STEP_TEST(0xec, "$ec CPX (absolute)")
DEFINE_OPCODE_STEP_TEST(0xed, "$ed SBC (absolute)")
DEFINE_OPCODE_STEP_TEST(0xee, "$ee INC (absolute)")
DEFINE_OPCODE_STEP_TEST(0xf0, "$f0 BEQ (relative)")
DEFINE_OPCODE_STEP_TEST(0xf1, "$f1 SBC (indirect,Y)")
DEFINE_OPCODE_STEP_TEST(0xf5, "$f5 SBC (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0xf6, "$f6 INC (zero page,X)")
DEFINE_OPCODE_STEP_TEST(0xf8, "$f8 SED (implied)")
DEFINE_OPCODE_STEP_TEST(0xf9, "$f9 SBC (absolute,Y)")
DEFINE_OPCODE_STEP_TEST(0xfd, "$fd SBC (absolute,X)")
DEFINE_OPCODE_STEP_TEST(0xfe, "$fe INC (absolute,X)")

#undef DEFINE_OPCODE_STEP_TEST
