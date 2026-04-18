#include "NESEmuCore/cpu6502.hpp"

#include <stdexcept>

#include "NESEmuCore/bus.hpp"

using namespace NESEmu;

Cpu6502::Cpu6502(Bus& bus)
    : m_bus(bus)
{
    m_opcodeHandlers[0x00] = &Cpu6502::opBRK;
    m_opcodeHandlers[0x01] = &Cpu6502::opORA_ind_X;
    m_opcodeHandlers[0x02] = &Cpu6502::opInvalid<0x02>;
    m_opcodeHandlers[0x03] = &Cpu6502::opInvalid<0x03>;
    m_opcodeHandlers[0x04] = &Cpu6502::opInvalid<0x04>;
    m_opcodeHandlers[0x05] = &Cpu6502::opORA_zp;
    m_opcodeHandlers[0x06] = &Cpu6502::opASL_zp;
    m_opcodeHandlers[0x07] = &Cpu6502::opInvalid<0x07>;
    m_opcodeHandlers[0x08] = &Cpu6502::opPHP;
    m_opcodeHandlers[0x09] = &Cpu6502::opORA_imm;
    m_opcodeHandlers[0x0A] = &Cpu6502::opASL_acc;
    m_opcodeHandlers[0x0B] = &Cpu6502::opInvalid<0x0B>;
    m_opcodeHandlers[0x0C] = &Cpu6502::opInvalid<0x0C>;
    m_opcodeHandlers[0x0D] = &Cpu6502::opORA_abs;
    m_opcodeHandlers[0x0E] = &Cpu6502::opASL_abs;
    m_opcodeHandlers[0x0F] = &Cpu6502::opInvalid<0x0F>;

    m_opcodeHandlers[0x10] = &Cpu6502::opBPL;
    m_opcodeHandlers[0x11] = &Cpu6502::opORA_ind_Y;
    m_opcodeHandlers[0x12] = &Cpu6502::opInvalid<0x12>;
    m_opcodeHandlers[0x13] = &Cpu6502::opInvalid<0x13>;
    m_opcodeHandlers[0x14] = &Cpu6502::opInvalid<0x14>;
    m_opcodeHandlers[0x15] = &Cpu6502::opORA_zp_X;
    m_opcodeHandlers[0x16] = &Cpu6502::opASL_zp_X;
    m_opcodeHandlers[0x17] = &Cpu6502::opInvalid<0x17>;
    m_opcodeHandlers[0x18] = &Cpu6502::opCLC;
    m_opcodeHandlers[0x19] = &Cpu6502::opORA_abs_Y;
    m_opcodeHandlers[0x1A] = &Cpu6502::opInvalid<0x1A>;
    m_opcodeHandlers[0x1B] = &Cpu6502::opInvalid<0x1B>;
    m_opcodeHandlers[0x1C] = &Cpu6502::opInvalid<0x1C>;
    m_opcodeHandlers[0x1D] = &Cpu6502::opORA_abs_X;
    m_opcodeHandlers[0x1E] = &Cpu6502::opASL_abs_X;
    m_opcodeHandlers[0x1F] = &Cpu6502::opInvalid<0x1F>;

    m_opcodeHandlers[0x20] = &Cpu6502::opJSR;
    m_opcodeHandlers[0x21] = &Cpu6502::opAND_ind_X;
    m_opcodeHandlers[0x22] = &Cpu6502::opInvalid<0x22>;
    m_opcodeHandlers[0x23] = &Cpu6502::opInvalid<0x23>;
    m_opcodeHandlers[0x24] = &Cpu6502::opBIT_zp;
    m_opcodeHandlers[0x25] = &Cpu6502::opAND_zp;
    m_opcodeHandlers[0x26] = &Cpu6502::opROL_zp;
    m_opcodeHandlers[0x27] = &Cpu6502::opInvalid<0x27>;
    m_opcodeHandlers[0x28] = &Cpu6502::opPLP;
    m_opcodeHandlers[0x29] = &Cpu6502::opAND_imm;
    m_opcodeHandlers[0x2A] = &Cpu6502::opROL_acc;
    m_opcodeHandlers[0x2B] = &Cpu6502::opInvalid<0x2B>;
    m_opcodeHandlers[0x2C] = &Cpu6502::opBIT_abs;
    m_opcodeHandlers[0x2D] = &Cpu6502::opAND_abs;
    m_opcodeHandlers[0x2E] = &Cpu6502::opROL_abs;
    m_opcodeHandlers[0x2F] = &Cpu6502::opInvalid<0x2F>;

    m_opcodeHandlers[0x30] = &Cpu6502::opBMI;
    m_opcodeHandlers[0x31] = &Cpu6502::opAND_ind_Y;
    m_opcodeHandlers[0x32] = &Cpu6502::opInvalid<0x32>;
    m_opcodeHandlers[0x33] = &Cpu6502::opInvalid<0x33>;
    m_opcodeHandlers[0x34] = &Cpu6502::opInvalid<0x34>;
    m_opcodeHandlers[0x35] = &Cpu6502::opAND_zp_X;
    m_opcodeHandlers[0x36] = &Cpu6502::opROL_zp_X;
    m_opcodeHandlers[0x37] = &Cpu6502::opInvalid<0x37>;
    m_opcodeHandlers[0x38] = &Cpu6502::opSEC;
    m_opcodeHandlers[0x39] = &Cpu6502::opAND_abs_Y;
    m_opcodeHandlers[0x3A] = &Cpu6502::opInvalid<0x3A>;
    m_opcodeHandlers[0x3B] = &Cpu6502::opInvalid<0x3B>;
    m_opcodeHandlers[0x3C] = &Cpu6502::opInvalid<0x3C>;
    m_opcodeHandlers[0x3D] = &Cpu6502::opAND_abs_X;
    m_opcodeHandlers[0x3E] = &Cpu6502::opROL_abs_X;
    m_opcodeHandlers[0x3F] = &Cpu6502::opInvalid<0x3F>;

    m_opcodeHandlers[0x40] = &Cpu6502::opRTI;
    m_opcodeHandlers[0x41] = &Cpu6502::opEOR_ind_X;
    m_opcodeHandlers[0x42] = &Cpu6502::opInvalid<0x42>;
    m_opcodeHandlers[0x43] = &Cpu6502::opInvalid<0x43>;
    m_opcodeHandlers[0x44] = &Cpu6502::opInvalid<0x44>;
    m_opcodeHandlers[0x45] = &Cpu6502::opEOR_zp;
    m_opcodeHandlers[0x46] = &Cpu6502::opLSR_zp;
    m_opcodeHandlers[0x47] = &Cpu6502::opInvalid<0x47>;
    m_opcodeHandlers[0x48] = &Cpu6502::opPHA;
    m_opcodeHandlers[0x49] = &Cpu6502::opEOR_imm;
    m_opcodeHandlers[0x4A] = &Cpu6502::opLSR_acc;
    m_opcodeHandlers[0x4B] = &Cpu6502::opInvalid<0x4B>;
    m_opcodeHandlers[0x4C] = &Cpu6502::opJMP_abs;
    m_opcodeHandlers[0x4D] = &Cpu6502::opEOR_abs;
    m_opcodeHandlers[0x4E] = &Cpu6502::opLSR_abs;
    m_opcodeHandlers[0x4F] = &Cpu6502::opInvalid<0x4F>;

    m_opcodeHandlers[0x50] = &Cpu6502::opBVC;
    m_opcodeHandlers[0x51] = &Cpu6502::opEOR_ind_Y;
    m_opcodeHandlers[0x52] = &Cpu6502::opInvalid<0x52>;
    m_opcodeHandlers[0x53] = &Cpu6502::opInvalid<0x53>;
    m_opcodeHandlers[0x54] = &Cpu6502::opInvalid<0x54>;
    m_opcodeHandlers[0x55] = &Cpu6502::opEOR_zp_X;
    m_opcodeHandlers[0x56] = &Cpu6502::opLSR_zp_X;
    m_opcodeHandlers[0x57] = &Cpu6502::opInvalid<0x57>;
    m_opcodeHandlers[0x58] = &Cpu6502::opCLI;
    m_opcodeHandlers[0x59] = &Cpu6502::opEOR_abs_Y;
    m_opcodeHandlers[0x5A] = &Cpu6502::opInvalid<0x5A>;
    m_opcodeHandlers[0x5B] = &Cpu6502::opInvalid<0x5B>;
    m_opcodeHandlers[0x5C] = &Cpu6502::opInvalid<0x5C>;
    m_opcodeHandlers[0x5D] = &Cpu6502::opEOR_abs_X;
    m_opcodeHandlers[0x5E] = &Cpu6502::opLSR_abs_X;
    m_opcodeHandlers[0x5F] = &Cpu6502::opInvalid<0x5F>;

    m_opcodeHandlers[0x60] = &Cpu6502::opRTS;
    m_opcodeHandlers[0x61] = &Cpu6502::opADC_ind_X;
    m_opcodeHandlers[0x62] = &Cpu6502::opInvalid<0x62>;
    m_opcodeHandlers[0x63] = &Cpu6502::opInvalid<0x63>;
    m_opcodeHandlers[0x64] = &Cpu6502::opInvalid<0x64>;
    m_opcodeHandlers[0x65] = &Cpu6502::opADC_zp;
    m_opcodeHandlers[0x66] = &Cpu6502::opROR_zp;
    m_opcodeHandlers[0x67] = &Cpu6502::opInvalid<0x67>;
    m_opcodeHandlers[0x68] = &Cpu6502::opPLA;
    m_opcodeHandlers[0x69] = &Cpu6502::opADC_imm;
    m_opcodeHandlers[0x6A] = &Cpu6502::opROR_acc;
    m_opcodeHandlers[0x6B] = &Cpu6502::opInvalid<0x6B>;
    m_opcodeHandlers[0x6C] = &Cpu6502::opJMP_ind;
    m_opcodeHandlers[0x6D] = &Cpu6502::opADC_abs;
    m_opcodeHandlers[0x6E] = &Cpu6502::opROR_abs;
    m_opcodeHandlers[0x6F] = &Cpu6502::opInvalid<0x6F>;

    m_opcodeHandlers[0x70] = &Cpu6502::opBVS;
    m_opcodeHandlers[0x71] = &Cpu6502::opADC_ind_Y;
    m_opcodeHandlers[0x72] = &Cpu6502::opInvalid<0x72>;
    m_opcodeHandlers[0x73] = &Cpu6502::opInvalid<0x73>;
    m_opcodeHandlers[0x74] = &Cpu6502::opInvalid<0x74>;
    m_opcodeHandlers[0x75] = &Cpu6502::opADC_zp_X;
    m_opcodeHandlers[0x76] = &Cpu6502::opROR_zp_X;
    m_opcodeHandlers[0x77] = &Cpu6502::opInvalid<0x77>;
    m_opcodeHandlers[0x78] = &Cpu6502::opSEI;
    m_opcodeHandlers[0x79] = &Cpu6502::opADC_abs_Y;
    m_opcodeHandlers[0x7A] = &Cpu6502::opInvalid<0x7A>;
    m_opcodeHandlers[0x7B] = &Cpu6502::opInvalid<0x7B>;
    m_opcodeHandlers[0x7C] = &Cpu6502::opInvalid<0x7C>;
    m_opcodeHandlers[0x7D] = &Cpu6502::opADC_abs_X;
    m_opcodeHandlers[0x7E] = &Cpu6502::opROR_abs_X;
    m_opcodeHandlers[0x7F] = &Cpu6502::opInvalid<0x7F>;

    m_opcodeHandlers[0x80] = &Cpu6502::opInvalid<0x80>;
    m_opcodeHandlers[0x81] = &Cpu6502::opSTA_ind_X;
    m_opcodeHandlers[0x82] = &Cpu6502::opInvalid<0x82>;
    m_opcodeHandlers[0x83] = &Cpu6502::opInvalid<0x83>;
    m_opcodeHandlers[0x84] = &Cpu6502::opSTY_zp;
    m_opcodeHandlers[0x85] = &Cpu6502::opSTA_zp;
    m_opcodeHandlers[0x86] = &Cpu6502::opSTX_zp;
    m_opcodeHandlers[0x87] = &Cpu6502::opInvalid<0x87>;
    m_opcodeHandlers[0x88] = &Cpu6502::opDEY;
    m_opcodeHandlers[0x89] = &Cpu6502::opInvalid<0x89>;
    m_opcodeHandlers[0x8A] = &Cpu6502::opTXA;
    m_opcodeHandlers[0x8B] = &Cpu6502::opInvalid<0x8B>;
    m_opcodeHandlers[0x8C] = &Cpu6502::opSTY_abs;
    m_opcodeHandlers[0x8D] = &Cpu6502::opSTA_abs;
    m_opcodeHandlers[0x8E] = &Cpu6502::opSTX_abs;
    m_opcodeHandlers[0x8F] = &Cpu6502::opInvalid<0x8F>;

    m_opcodeHandlers[0x90] = &Cpu6502::opBCC;
    m_opcodeHandlers[0x91] = &Cpu6502::opSTA_ind_Y;
    m_opcodeHandlers[0x92] = &Cpu6502::opInvalid<0x92>;
    m_opcodeHandlers[0x93] = &Cpu6502::opInvalid<0x93>;
    m_opcodeHandlers[0x94] = &Cpu6502::opSTY_zp_X;
    m_opcodeHandlers[0x95] = &Cpu6502::opSTA_zp_X;
    m_opcodeHandlers[0x96] = &Cpu6502::opSTX_zp_Y;
    m_opcodeHandlers[0x97] = &Cpu6502::opInvalid<0x97>;
    m_opcodeHandlers[0x98] = &Cpu6502::opTYA;
    m_opcodeHandlers[0x99] = &Cpu6502::opSTA_abs_Y;
    m_opcodeHandlers[0x9A] = &Cpu6502::opTXS;
    m_opcodeHandlers[0x9B] = &Cpu6502::opInvalid<0x9B>;
    m_opcodeHandlers[0x9C] = &Cpu6502::opInvalid<0x9C>;
    m_opcodeHandlers[0x9D] = &Cpu6502::opSTA_abs_X;
    m_opcodeHandlers[0x9E] = &Cpu6502::opInvalid<0x9E>;
    m_opcodeHandlers[0x9F] = &Cpu6502::opInvalid<0x9F>;

    m_opcodeHandlers[0xA0] = &Cpu6502::opLDY_imm;
    m_opcodeHandlers[0xA1] = &Cpu6502::opLDA_ind_X;
    m_opcodeHandlers[0xA2] = &Cpu6502::opLDX_imm;
    m_opcodeHandlers[0xA3] = &Cpu6502::opInvalid<0xA3>;
    m_opcodeHandlers[0xA4] = &Cpu6502::opLDY_zp;
    m_opcodeHandlers[0xA5] = &Cpu6502::opLDA_zp;
    m_opcodeHandlers[0xA6] = &Cpu6502::opLDX_zp;
    m_opcodeHandlers[0xA7] = &Cpu6502::opInvalid<0xA7>;
    m_opcodeHandlers[0xA8] = &Cpu6502::opTAY;
    m_opcodeHandlers[0xA9] = &Cpu6502::opLDA_imm;
    m_opcodeHandlers[0xAA] = &Cpu6502::opTAX;
    m_opcodeHandlers[0xAB] = &Cpu6502::opInvalid<0xAB>;
    m_opcodeHandlers[0xAC] = &Cpu6502::opLDY_abs;
    m_opcodeHandlers[0xAD] = &Cpu6502::opLDA_abs;
    m_opcodeHandlers[0xAE] = &Cpu6502::opLDX_abs;
    m_opcodeHandlers[0xAF] = &Cpu6502::opInvalid<0xAF>;

    m_opcodeHandlers[0xB0] = &Cpu6502::opBCS;
    m_opcodeHandlers[0xB1] = &Cpu6502::opLDA_ind_Y;
    m_opcodeHandlers[0xB2] = &Cpu6502::opInvalid<0xB2>;
    m_opcodeHandlers[0xB3] = &Cpu6502::opInvalid<0xB3>;
    m_opcodeHandlers[0xB4] = &Cpu6502::opLDY_zp_X;
    m_opcodeHandlers[0xB5] = &Cpu6502::opLDA_zp_X;
    m_opcodeHandlers[0xB6] = &Cpu6502::opLDX_zp_Y;
    m_opcodeHandlers[0xB7] = &Cpu6502::opInvalid<0xB7>;
    m_opcodeHandlers[0xB8] = &Cpu6502::opCLV;
    m_opcodeHandlers[0xB9] = &Cpu6502::opLDA_abs_Y;
    m_opcodeHandlers[0xBA] = &Cpu6502::opTSX;
    m_opcodeHandlers[0xBB] = &Cpu6502::opInvalid<0xBB>;
    m_opcodeHandlers[0xBC] = &Cpu6502::opLDY_abs_X;
    m_opcodeHandlers[0xBD] = &Cpu6502::opLDA_abs_X;
    m_opcodeHandlers[0xBE] = &Cpu6502::opLDX_abs_Y;
    m_opcodeHandlers[0xBF] = &Cpu6502::opInvalid<0xBF>;

    m_opcodeHandlers[0xC0] = &Cpu6502::opCPY_imm;
    m_opcodeHandlers[0xC1] = &Cpu6502::opCMP_ind_X;
    m_opcodeHandlers[0xC2] = &Cpu6502::opInvalid<0xC2>;
    m_opcodeHandlers[0xC3] = &Cpu6502::opInvalid<0xC3>;
    m_opcodeHandlers[0xC4] = &Cpu6502::opCPY_zp;
    m_opcodeHandlers[0xC5] = &Cpu6502::opCMP_zp;
    m_opcodeHandlers[0xC6] = &Cpu6502::opDEC_zp;
    m_opcodeHandlers[0xC7] = &Cpu6502::opInvalid<0xC7>;
    m_opcodeHandlers[0xC8] = &Cpu6502::opINY;
    m_opcodeHandlers[0xC9] = &Cpu6502::opCMP_imm;
    m_opcodeHandlers[0xCA] = &Cpu6502::opDEX;
    m_opcodeHandlers[0xCB] = &Cpu6502::opInvalid<0xCB>;
    m_opcodeHandlers[0xCC] = &Cpu6502::opCPY_abs;
    m_opcodeHandlers[0xCD] = &Cpu6502::opCMP_abs;
    m_opcodeHandlers[0xCE] = &Cpu6502::opDEC_abs;
    m_opcodeHandlers[0xCF] = &Cpu6502::opInvalid<0xCF>;

    m_opcodeHandlers[0xD0] = &Cpu6502::opBNE;
    m_opcodeHandlers[0xD1] = &Cpu6502::opCMP_ind_Y;
    m_opcodeHandlers[0xD2] = &Cpu6502::opInvalid<0xD2>;
    m_opcodeHandlers[0xD3] = &Cpu6502::opInvalid<0xD3>;
    m_opcodeHandlers[0xD4] = &Cpu6502::opInvalid<0xD4>;
    m_opcodeHandlers[0xD5] = &Cpu6502::opCMP_zp_X;
    m_opcodeHandlers[0xD6] = &Cpu6502::opDEC_zp_X;
    m_opcodeHandlers[0xD7] = &Cpu6502::opInvalid<0xD7>;
    m_opcodeHandlers[0xD8] = &Cpu6502::opCLD;
    m_opcodeHandlers[0xD9] = &Cpu6502::opCMP_abs_Y;
    m_opcodeHandlers[0xDA] = &Cpu6502::opInvalid<0xDA>;
    m_opcodeHandlers[0xDB] = &Cpu6502::opInvalid<0xDB>;
    m_opcodeHandlers[0xDC] = &Cpu6502::opInvalid<0xDC>;
    m_opcodeHandlers[0xDD] = &Cpu6502::opCMP_abs_X;
    m_opcodeHandlers[0xDE] = &Cpu6502::opDEC_abs_X;
    m_opcodeHandlers[0xDF] = &Cpu6502::opInvalid<0xDF>;

    m_opcodeHandlers[0xE0] = &Cpu6502::opCPX_imm;
    m_opcodeHandlers[0xE1] = &Cpu6502::opSBC_ind_X;
    m_opcodeHandlers[0xE2] = &Cpu6502::opInvalid<0xE2>;
    m_opcodeHandlers[0xE3] = &Cpu6502::opInvalid<0xE3>;
    m_opcodeHandlers[0xE4] = &Cpu6502::opCPX_zp;
    m_opcodeHandlers[0xE5] = &Cpu6502::opSBC_zp;
    m_opcodeHandlers[0xE6] = &Cpu6502::opINC_zp;
    m_opcodeHandlers[0xE7] = &Cpu6502::opInvalid<0xE7>;
    m_opcodeHandlers[0xE8] = &Cpu6502::opINX;
    m_opcodeHandlers[0xE9] = &Cpu6502::opSBC_imm;
    m_opcodeHandlers[0xEA] = &Cpu6502::opNOP;
    m_opcodeHandlers[0xEB] = &Cpu6502::opInvalid<0xEB>;
    m_opcodeHandlers[0xEC] = &Cpu6502::opCPX_abs;
    m_opcodeHandlers[0xED] = &Cpu6502::opSBC_abs;
    m_opcodeHandlers[0xEE] = &Cpu6502::opINC_abs;
    m_opcodeHandlers[0xEF] = &Cpu6502::opInvalid<0xEF>;

    m_opcodeHandlers[0xF0] = &Cpu6502::opBEQ;
    m_opcodeHandlers[0xF1] = &Cpu6502::opSBC_ind_Y;
    m_opcodeHandlers[0xF2] = &Cpu6502::opInvalid<0xF2>;
    m_opcodeHandlers[0xF3] = &Cpu6502::opInvalid<0xF3>;
    m_opcodeHandlers[0xF4] = &Cpu6502::opInvalid<0xF4>;
    m_opcodeHandlers[0xF5] = &Cpu6502::opSBC_zp_X;
    m_opcodeHandlers[0xF6] = &Cpu6502::opINC_zp_X;
    m_opcodeHandlers[0xF7] = &Cpu6502::opInvalid<0xF7>;
    m_opcodeHandlers[0xF8] = &Cpu6502::opSED;
    m_opcodeHandlers[0xF9] = &Cpu6502::opSBC_abs_Y;
    m_opcodeHandlers[0xFA] = &Cpu6502::opInvalid<0xFA>;
    m_opcodeHandlers[0xFB] = &Cpu6502::opInvalid<0xFB>;
    m_opcodeHandlers[0xFC] = &Cpu6502::opInvalid<0xFC>;
    m_opcodeHandlers[0xFD] = &Cpu6502::opSBC_abs_X;
    m_opcodeHandlers[0xFE] = &Cpu6502::opINC_abs_X;
    m_opcodeHandlers[0xFF] = &Cpu6502::opInvalid<0xFF>;
}

Cpu6502::~Cpu6502() = default;

void Cpu6502::startup()
{
    // https://www.nesdev.org/wiki/Init_code

    const uint8 lo = readMemory(kVecReset);
    const uint8 hi = readMemory(kVecReset + 1);
    m_address      = hi << 8 | lo;

    m_state.pc = m_address;
    m_state.sp = 0xFD;
    m_state.a  = 0;
    m_state.x  = 0;
    m_state.y  = 0;
    m_state.p  = 0;
    setRegister(U, true);
}

void Cpu6502::reset()
{
    m_state.pc = readMemory(kVecReset);
    m_state.sp -= 3;

    // Other registers remain unchanged on reset
}

void Cpu6502::execute()
{
    const uint8 opcode = readMemory(m_state.pc++);

    (this->*m_opcodeHandlers[opcode])();
}

uint8 Cpu6502::readMemory(const uint16 address)
{
    m_cycles++;
    m_address = address;

    return m_data = m_bus.read(address);
}

void Cpu6502::writeMemory(const uint16 address, const uint8 value)
{
    m_cycles++;
    m_address = address;
    m_data    = value;
    m_bus.write(address, value);
}

void Cpu6502::pushStack(uint8 value)
{
    writeMemory(kStackAddr + m_state.sp, value);
    m_state.sp--;
}

uint8 Cpu6502::popStack()
{
    ++m_state.sp;
    return readMemory(kStackAddr + m_state.sp);
}

void Cpu6502::addressModeImplied()
{
    // implied addressing mode takes an extra cycle
    m_cycles++;
}

void Cpu6502::addressModeImmediate()
{
    readMemory(m_state.pc++);
}

void Cpu6502::addressModeRelative()
{
    readMemory(m_state.pc++);
}

template <int MODE>
void Cpu6502::addressModeZeroPage()
{
    m_address = readMemory(m_state.pc++);

    if constexpr (kModeReadOnly == MODE) {
        readMemory(m_address);
    }
}

template <int MODE>
void Cpu6502::addressModeZeroPageX()
{
    m_address = static_cast<uint8>(readMemory(m_state.pc++) + m_state.x);

    if constexpr (kModeWriteOnly != MODE) {
        readMemory(m_address);
    }

    // Zero page indexing takes an extra internal cycle, so the ALU
    // has time to add the value of the register to the base address
    m_cycles++;
}

template <int MODE>
void Cpu6502::addressModeZeroPageY()
{
    m_address = static_cast<uint8>(readMemory(m_state.pc++) + m_state.y);

    if constexpr (kModeWriteOnly != MODE) {
        readMemory(m_address);
    }

    // Zero page indexing takes an extra internal cycle, so the ALU
    // has time to add the value of the register to the base address
    m_cycles++;
}

template <int MODE>
void Cpu6502::addressModeAbsolute()
{
    const uint8 lo = readMemory(m_state.pc++);
    const uint8 hi = readMemory(m_state.pc++);
    m_address      = hi << 8 | lo;

    if constexpr (kModeWriteOnly != MODE) {
        readMemory(m_address);
    }
}

template <int MODE>
void Cpu6502::addressModeAbsoluteX()
{
    const uint8  lo   = readMemory(m_state.pc++);
    const uint8  hi   = readMemory(m_state.pc++);
    const uint16 base = hi << 8 | lo;
    m_address         = base + m_state.x;

    // A note on the extra cycle taken when crossing a page boundry:
    // https://retrocomputing.stackexchange.com/a/15623
    // The CPU needs to do an addition of the 2-byte address after the opcode and the 8-bit unsigned displacement from X or Y register.
    // Since 6502 addresses are always stored as little-endian, the CPU gets the lower byte first. During the time it reads the following higher byte, it simultaneously performs addition of the lower byte just read and the contents of the index register.
    // If that addition results in no carry, the CPU instantly knows it does not need to update the high byte of the address. Therefore the read cycle at the correct address follows (no penalty).
    // When there is carry, the 6502 needs another cycle to increment the high byte of the address. During that process, the 6502 does a dummy read cycle at the incorrect address, followed by the correct read cycle (penalty).
    // It is obvious that there should be no 'dummy' write cycles at incorrect addresses. Therefore the 6502 always takes a 'penalty' read cycle with an (probably) incorrect address before performing the final write.
    //
    // To summarize, these indexed full address operations should always take an extra cycle, but
    // the reads have been optimized to take only one cycle when the upper address bits remain unchanged.

    if constexpr (kModeReadOnly == MODE || kModeReadWrite == MODE) {
        readMemory(m_address);

        if constexpr (kModeReadWrite == MODE) {
            m_cycles++;
        } else {
            if (lo + m_state.x > 0xFF) {
                // Crossing a page boundry takes an extra internal cycle
                m_cycles++;
            }
        }
    }
    if constexpr (kModeWriteOnly == MODE) {
        m_cycles++;
    }
}

template <int MODE>
void Cpu6502::addressModeAbsoluteY()
{
    const uint8  lo   = readMemory(m_state.pc++);
    const uint8  hi   = readMemory(m_state.pc++);
    const uint16 base = hi << 8 | lo;
    m_address         = base + m_state.y;

    if constexpr (kModeReadOnly == MODE || kModeReadWrite == MODE) {
        readMemory(m_address);

        if constexpr (kModeReadWrite == MODE) {
            m_cycles++;
        } else {
            if (lo + m_state.y > 0xFF) {
                // Crossing a page boundry takes an extra internal cycle
                m_cycles++;
            }
        }
    }
    if constexpr (kModeWriteOnly == MODE) {
        m_cycles++;
    }
}

template <int MODE>
void Cpu6502::addressModeIndirectX()
{
    uint8       zp = readMemory(m_state.pc++) + m_state.x;
    const uint8 lo = readMemory(zp++);
    const uint8 hi = readMemory(zp);
    m_address      = hi << 8 | lo;

    if constexpr (kModeWriteOnly != MODE) {
        readMemory(m_address);
    }

    // Index mode takes an extra internal cycle
    m_cycles++;
}

template <int MODE>
void Cpu6502::addressModeIndirectY()
{
    uint8        zp   = readMemory(m_state.pc++);
    const uint8  lo   = readMemory(zp++);
    const uint8  hi   = readMemory(zp);
    const uint16 base = hi << 8 | lo;
    m_address         = base + m_state.y;

    if constexpr (kModeReadOnly == MODE || kModeReadWrite == MODE) {
        readMemory(m_address);

        if (lo + m_state.y > 0xFF) {
            // Crossing a page boundry takes an extra internal cycle
            m_cycles++;
        }
    }
    if constexpr (kModeWriteOnly == MODE) {
        m_cycles++;
    }
}

template <unsigned OP>
void Cpu6502::opInvalid()
{
    // TODO: Log invalid opcode (and halt execution?)
    throw std::runtime_error("Invalid opcode");
}

void Cpu6502::opADC()
{
    const uint8  carry = getRegister(C);
    const uint16 total = m_state.a + m_data + carry;
    const auto   data  = static_cast<uint8>(total);

    setRegister(C, total > 0xFF);
    setRegister(Z, !data);
    setRegister(V, (data ^ m_state.a) & (data ^ m_data) & 0x80);
    setRegister(N, data & 0x80);

    m_state.a = data;
}

void Cpu6502::opADC_imm()
{
    addressModeImmediate();
    opADC();
}

void Cpu6502::opADC_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opADC();
}

void Cpu6502::opADC_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opADC();
}

void Cpu6502::opAND()
{
    const uint8 data = m_state.a & m_data;

    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_state.a = data;
}

void Cpu6502::opAND_imm()
{
    addressModeImmediate();
    opAND();
}

void Cpu6502::opAND_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opAND();
}

void Cpu6502::opAND_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opAND();
}

void Cpu6502::opASL()
{
    const uint16 total = m_data << 1;
    const auto   data  = static_cast<uint8>(total);

    // ASL takes an extra cycle to complete operation
    m_cycles++;

    setRegister(C, total & 0x100);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_data = data;
}

void Cpu6502::opASL_acc()
{
    m_data = m_state.a;
    opASL();
    m_state.a = m_data;
}

void Cpu6502::opASL_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opASL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opASL_zp_X()
{
    addressModeZeroPageX<kModeReadWrite>();
    opASL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opASL_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opASL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opASL_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opASL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opBRC(bool branch)
{
    if (branch) {
        const auto lo     = static_cast<uint8>(m_state.pc);
        const auto offset = static_cast<sint8>(m_data);

        if (lo + offset & 0xFF00) {
            // Crossing a page boundry takes an extra internal cycle
            m_cycles++;
        }

        // Jump takes a cycle
        m_cycles++;

        m_state.pc += offset;
    }
}

void Cpu6502::opBCC()
{
    addressModeRelative();
    opBRC(!getRegister(C));
}

void Cpu6502::opBCS()
{
    addressModeRelative();
    opBRC(getRegister(C));
}

void Cpu6502::opBEQ()
{
    addressModeRelative();
    opBRC(getRegister(Z));
}

void Cpu6502::opBIT()
{
    const uint8 data = m_state.a & m_data;

    setRegister(Z, !data);
    setRegister(V, m_data & 0x40);
    setRegister(N, m_data & 0x80);
}

void Cpu6502::opBIT_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opBIT();
}

void Cpu6502::opBIT_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opBIT();
}

void Cpu6502::opBMI()
{
    addressModeRelative();
    opBRC(getRegister(N));
}

void Cpu6502::opBNE()
{
    addressModeRelative();
    opBRC(!getRegister(Z));
}

void Cpu6502::opBPL()
{
    addressModeRelative();
    opBRC(!getRegister(N));
}

void Cpu6502::opBRK()
{
    addressModeImplied();

    const uint16 pc = m_state.pc + 1;
    const uint8  p  = m_state.p & 0b11001111 | 0b00110000;

    pushStack(static_cast<uint8>(pc >> 8));
    pushStack(static_cast<uint8>(pc));
    pushStack(p);

    setRegister(I, true);

    const uint8 lo = readMemory(kVecIrq);
    const uint8 hi = readMemory(kVecIrq + 1);
    m_state.pc     = hi << 8 | lo;
}

void Cpu6502::opBVC()
{
    addressModeRelative();
    opBRC(!getRegister(V));
}

void Cpu6502::opBVS()
{
    addressModeRelative();
    opBRC(getRegister(V));
}

void Cpu6502::opCLC()
{
    addressModeImplied();
    setRegister(C, false);
}

void Cpu6502::opCLD()
{
    addressModeImplied();
    setRegister(D, false);
}

void Cpu6502::opCLI()
{
    addressModeImplied();
    setRegister(I, false);
}

void Cpu6502::opCLV()
{
    addressModeImplied();
    setRegister(V, false);
}

void Cpu6502::opCMP()
{
    const uint8 data = m_state.a - m_data;

    setRegister(C, m_state.a >= m_data);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);
}

void Cpu6502::opCMP_imm()
{
    addressModeImmediate();
    opCMP();
}

void Cpu6502::opCMP_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCMP_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opCMP();
}

void Cpu6502::opCPX()
{
    const uint8 data = m_state.x - m_data;

    setRegister(C, m_state.x >= m_data);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);
}

void Cpu6502::opCPX_imm()
{
    addressModeImmediate();
    opCPX();
}

void Cpu6502::opCPX_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opCPX();
}

void Cpu6502::opCPX_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opCPX();
}

void Cpu6502::opCPY()
{
    const uint8 data = m_state.y - m_data;

    setRegister(C, m_state.y >= m_data);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);
}

void Cpu6502::opCPY_imm()
{
    addressModeImmediate();
    opCPY();
}

void Cpu6502::opCPY_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opCPY();
}

void Cpu6502::opCPY_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opCPY();
}

void Cpu6502::opDEC()
{
    const uint8 data = m_data - 1;

    // DEC takes an extra cycle to complete operation
    m_cycles++;

    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_data = data;
}

void Cpu6502::opDEC_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opDEC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opDEC_zp_X()
{
    addressModeZeroPageX<kModeReadWrite>();
    opDEC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opDEC_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opDEC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opDEC_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opDEC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opDEX()
{
    addressModeImplied();

    m_state.x--;

    setRegister(Z, !m_state.x);
    setRegister(N, m_state.x & 0x80);
}

void Cpu6502::opDEY()
{
    addressModeImplied();

    m_state.y--;

    setRegister(Z, !m_state.y);
    setRegister(N, m_state.y & 0x80);
}

void Cpu6502::opEOR()
{
    m_state.a ^= m_data;

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

void Cpu6502::opEOR_imm()
{
    addressModeImmediate();
    opEOR();
}

void Cpu6502::opEOR_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opEOR_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opEOR();
}

void Cpu6502::opINC()
{
    const uint8 data = m_data + 1;

    // INC takes an extra cycle to complete operation
    m_cycles++;

    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_data = data;
}

void Cpu6502::opINC_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opINC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opINC_zp_X()
{
    addressModeZeroPageX<kModeReadWrite>();
    opINC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opINC_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opINC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opINC_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opINC();
    writeMemory(m_address, m_data);
}

void Cpu6502::opINX()
{
    addressModeImplied();

    m_state.x++;

    setRegister(N, m_state.x & 0x80);
    setRegister(Z, !m_state.x);
}

void Cpu6502::opINY()
{
    addressModeImplied();

    m_state.y++;

    setRegister(N, m_state.y & 0x80);
    setRegister(Z, !m_state.y);
}

void Cpu6502::opJMP_abs()
{
    const uint8  lo   = readMemory(m_state.pc++);
    const uint8  hi   = readMemory(m_state.pc++);
    const uint16 addr = hi << 8 | lo;

    m_state.pc = addr;
}

void Cpu6502::opJMP_ind()
{
    const uint8  lo  = readMemory(m_state.pc++);
    const uint8  hi  = readMemory(m_state.pc++);
    const uint16 ptr = hi << 8 | lo;

    // Be sure to emulate the indirect jump page wrap bug
    const uint8  plo  = readMemory(ptr);
    const uint8  phi  = readMemory(hi << 8 | static_cast<uint8>(lo + 1));
    const uint16 addr = phi << 8 | plo;

    m_state.pc = addr;
}

void Cpu6502::opJSR()
{
    const uint16 pc   = m_state.pc + 1;
    const uint8  lo   = readMemory(m_state.pc++);
    const uint8  hi   = readMemory(m_state.pc++);
    const uint16 addr = hi << 8 | lo;

    pushStack(static_cast<uint8>(pc >> 8));
    pushStack(static_cast<uint8>(pc));

    // JSR takes an extra cycle to complete operation
    m_cycles++;

    m_state.pc = addr;
}

void Cpu6502::opLDA()
{
    m_state.a = m_data;

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

void Cpu6502::opLDA_imm()
{
    addressModeImmediate();
    opLDA();
}

void Cpu6502::opLDA_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDA_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opLDA();
}

void Cpu6502::opLDX()
{
    m_state.x = m_data;

    setRegister(Z, !m_state.x);
    setRegister(N, m_state.x & 0x80);
}

void Cpu6502::opLDX_imm()
{
    addressModeImmediate();
    opLDX();
}

void Cpu6502::opLDX_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opLDX();
}

void Cpu6502::opLDX_zp_Y()
{
    addressModeZeroPageY<kModeReadOnly>();
    opLDX();
}

void Cpu6502::opLDX_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opLDX();
}

void Cpu6502::opLDX_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opLDX();
}

void Cpu6502::opLDY()
{
    m_state.y = m_data;

    setRegister(Z, !m_state.y);
    setRegister(N, m_state.y & 0x80);
}

void Cpu6502::opLDY_imm()
{
    addressModeImmediate();
    opLDY();
}

void Cpu6502::opLDY_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opLDY();
}

void Cpu6502::opLDY_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opLDY();
}

void Cpu6502::opLDY_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opLDY();
}

void Cpu6502::opLDY_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opLDY();
}

void Cpu6502::opLSR()
{
    const uint16 total = m_data >> 1;
    const auto   data  = static_cast<uint8>(total);

    // LSR takes an extra cycle to complete operation
    m_cycles++;

    setRegister(C, m_data & 0x01);
    setRegister(Z, !data);
    setRegister(N, false);

    m_data = data;
}

void Cpu6502::opLSR_acc()
{
    m_data = m_state.a;
    opLSR();
    m_state.a = m_data;
}

void Cpu6502::opLSR_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opLSR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opLSR_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opLSR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opLSR_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opLSR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opLSR_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opLSR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opNOP()
{
    // NOP has a dummy read which takes an extra cycle
    m_cycles++;
}

void Cpu6502::opORA()
{
    m_state.a |= m_data;

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

void Cpu6502::opORA_imm()
{
    addressModeImmediate();
    opORA();
}

void Cpu6502::opORA_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opORA();
}

void Cpu6502::opORA_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opORA();
}

void Cpu6502::opPHA()
{
    addressModeImplied();
    pushStack(m_state.a);
}

void Cpu6502::opPHP()
{
    const uint8 p = m_state.p | B;

    addressModeImplied();
    pushStack(p);
}

void Cpu6502::opPLA()
{
    addressModeImplied();

    // PLA takes an extra cycle
    m_cycles++;

    m_state.a = popStack();

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

void Cpu6502::opPLP()
{
    const uint8 p = popStack() & 0b11001111;

    addressModeImplied();

    // PLA takes an extra cycle
    m_cycles++;

    // Note that the effect of changing I is delayed one instruction because the flag is changed
    // after IRQ is polled, delaying the effect until IRQ is polled in the next instruction
    // TODO: Ensure this behavior is emulated correctly
    m_state.p = p | U;
}

void Cpu6502::opROL()
{
    const uint16 total = m_data << 1 | getRegister(C);
    const auto   data  = static_cast<uint8>(total);

    // ROL takes an extra cycle to complete operation
    m_cycles++;

    setRegister(C, m_data & 0x80);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_data = data;
}

void Cpu6502::opROL_acc()
{
    m_data = m_state.a;
    opROL();
    m_state.a = m_data;
}

void Cpu6502::opROL_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opROL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROL_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opROL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROL_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opROL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROL_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opROL();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROR()
{
    const uint16 total = m_data >> 1 | (getRegister(C) << 7);
    const auto   data  = static_cast<uint8>(total);

    // ROR takes an extra cycle to complete operation
    m_cycles++;

    setRegister(C, m_data & 0x01);
    setRegister(Z, !data);
    setRegister(N, data & 0x80);

    m_data = data;
}

void Cpu6502::opROR_acc()
{
    m_data = m_state.a;
    opROR();
    m_state.a = m_data;
}

void Cpu6502::opROR_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opROR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROR_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opROR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROR_abs()
{
    addressModeAbsolute<kModeReadWrite>();
    opROR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opROR_abs_X()
{
    addressModeAbsoluteX<kModeReadWrite>();
    opROR();
    writeMemory(m_address, m_data);
}

void Cpu6502::opRTI()
{
    addressModeImplied();

    const uint8  p  = popStack() & 0b11001111 | 0b00100000;
    const uint8  lo = popStack();
    const uint8  hi = popStack();
    const uint16 pc = hi << 8 | lo;

    m_state.p  = p;
    m_state.pc = pc;

    m_cycles++;
}

void Cpu6502::opRTS()
{
    const uint8  lo = popStack();
    const uint8  hi = popStack();
    const uint16 pc = hi << 8 | lo;

    // JSR performs several dummy reads during this instruction
    // An interesting discussion that outlines exactly why this
    // takes 6 cycles despite seemingly few memory operations
    // https://6502.org/forum/viewtopic.php?f=2&t=5146

    // Cycle 1: fetch RTS - the CPU can't do anything else as it hasn't seen the instruction
    // Cycle 2: fetch possible operand, as it always does, and must, because it hasn't had time to decode the instruction. But it does send SP to the ALU, and also to the address bus
    // Cycle 3: SP to address bus, read the empty byte below the stack, not useful but it has to do something. ALU performs the increment for the next cycle
    // Cycle 4: SP+1 to address bus, reading PC-1 low byte, will be directed to ALU. ALU performs another increment
    // Cycle 5: SP+2 to address bus from the ALU, also updates the value of SP. ALU performs a NOP with PC-1 low byte, PC-1 high byte is read
    // Cycle 6: PC is updated with PC-1, new PC sent to address bus, byte before destination is read because something has to happen, PC will be incremented as usual
    m_cycles += 3;

    m_state.pc = pc + 1;
}

void Cpu6502::opSBC()
{
    const uint8 carry  = 1 - getRegister(C);
    const auto  result = static_cast<uint8>(m_state.a - m_data - carry);

    setRegister(C, m_state.a >= m_data + carry);
    setRegister(Z, !result);
    setRegister(V, (result ^ m_state.a) & (result ^ ~m_data) & 0x80);
    setRegister(N, result & 0x80);

    m_state.a = result;
}

void Cpu6502::opSBC_imm()
{
    addressModeImmediate();
    opSBC();
}

void Cpu6502::opSBC_zp()
{
    addressModeZeroPage<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_zp_X()
{
    addressModeZeroPageX<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_abs()
{
    addressModeAbsolute<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_abs_X()
{
    addressModeAbsoluteX<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_abs_Y()
{
    addressModeAbsoluteY<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_ind_X()
{
    addressModeIndirectX<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSBC_ind_Y()
{
    addressModeIndirectY<kModeReadOnly>();
    opSBC();
}

void Cpu6502::opSEC()
{
    addressModeImplied();
    setRegister(C, true);
}

void Cpu6502::opSED()
{
    addressModeImplied();
    setRegister(D, true);
}

void Cpu6502::opSEI()
{
    addressModeImplied();
    setRegister(I, true);
}

void Cpu6502::opSTA()
{
    writeMemory(m_address, m_state.a);
}

void Cpu6502::opSTA_zp()
{
    addressModeZeroPage<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_zp_X()
{
    addressModeZeroPageX<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_abs()
{
    addressModeAbsolute<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_abs_X()
{
    addressModeAbsoluteX<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_abs_Y()
{
    addressModeAbsoluteY<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_ind_X()
{
    addressModeIndirectX<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTA_ind_Y()
{
    addressModeIndirectY<kModeWriteOnly>();
    opSTA();
}

void Cpu6502::opSTX()
{
    writeMemory(m_address, m_state.x);
}

void Cpu6502::opSTX_zp()
{
    addressModeZeroPage<kModeWriteOnly>();
    opSTX();
}

void Cpu6502::opSTX_zp_Y()
{
    addressModeZeroPageY<kModeWriteOnly>();
    opSTX();
}

void Cpu6502::opSTX_abs()
{
    addressModeAbsolute<kModeWriteOnly>();
    opSTX();
}

void Cpu6502::opSTY()
{
    writeMemory(m_address, m_state.y);
}

void Cpu6502::opSTY_zp()
{
    addressModeZeroPage<kModeWriteOnly>();
    opSTY();
}

void Cpu6502::opSTY_zp_X()
{
    addressModeZeroPageX<kModeWriteOnly>();
    opSTY();
}

void Cpu6502::opSTY_abs()
{
    addressModeAbsolute<kModeWriteOnly>();
    opSTY();
}

void Cpu6502::opTAX()
{
    addressModeImplied();
    m_state.x = m_state.a;

    setRegister(Z, !m_state.x);
    setRegister(N, m_state.x & 0x80);
}

void Cpu6502::opTAY()
{
    addressModeImplied();
    m_state.y = m_state.a;

    setRegister(Z, !m_state.y);
    setRegister(N, m_state.y & 0x80);
}

void Cpu6502::opTSX()
{
    addressModeImplied();
    m_state.x = m_state.sp;

    setRegister(Z, !m_state.x);
    setRegister(N, m_state.x & 0x80);
}

void Cpu6502::opTXA()
{
    addressModeImplied();
    m_state.a = m_state.x;

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

void Cpu6502::opTXS()
{
    addressModeImplied();
    m_state.sp = m_state.x;
}

void Cpu6502::opTYA()
{
    addressModeImplied();
    m_state.a = m_state.y;

    setRegister(Z, !m_state.a);
    setRegister(N, m_state.a & 0x80);
}

bool NESEmu::operator==(const Cpu6502::State& lhs, const Cpu6502::State& rhs)
{
    return lhs.pc == rhs.pc &&
           lhs.sp == rhs.sp &&
           lhs.a == rhs.a &&
           lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.p == rhs.p;
}

bool NESEmu::operator!=(const Cpu6502::State& lhs, const Cpu6502::State& rhs)
{
    return !(lhs == rhs);
}
