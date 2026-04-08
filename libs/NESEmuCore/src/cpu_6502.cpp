#include "NESEmuCore/cpu_6502.hpp"

#include <stdexcept>

#include "NESEmuCore/bus.hpp"

using namespace NESEmu;

CPU_6502::CPU_6502(Bus* bus) :
    m_Bus(bus) {
    m_OpcodeHandlers[0x00] = &CPU_6502::opInvalid<0x00>;
    m_OpcodeHandlers[0x01] = &CPU_6502::opInvalid<0x01>;
    m_OpcodeHandlers[0x02] = &CPU_6502::opInvalid<0x02>;
    m_OpcodeHandlers[0x03] = &CPU_6502::opInvalid<0x03>;
    m_OpcodeHandlers[0x04] = &CPU_6502::opInvalid<0x04>;
    m_OpcodeHandlers[0x05] = &CPU_6502::opInvalid<0x05>;
    m_OpcodeHandlers[0x06] = &CPU_6502::opInvalid<0x06>;
    m_OpcodeHandlers[0x07] = &CPU_6502::opInvalid<0x07>;
    m_OpcodeHandlers[0x08] = &CPU_6502::opInvalid<0x08>;
    m_OpcodeHandlers[0x09] = &CPU_6502::opInvalid<0x09>;
    m_OpcodeHandlers[0x0A] = &CPU_6502::opInvalid<0x0A>;
    m_OpcodeHandlers[0x0B] = &CPU_6502::opInvalid<0x0B>;
    m_OpcodeHandlers[0x0C] = &CPU_6502::opInvalid<0x0C>;
    m_OpcodeHandlers[0x0D] = &CPU_6502::opInvalid<0x0D>;
    m_OpcodeHandlers[0x0E] = &CPU_6502::opInvalid<0x0E>;
    m_OpcodeHandlers[0x0F] = &CPU_6502::opInvalid<0x0F>;

    m_OpcodeHandlers[0x10] = &CPU_6502::opInvalid<0x10>;
    m_OpcodeHandlers[0x11] = &CPU_6502::opInvalid<0x11>;
    m_OpcodeHandlers[0x12] = &CPU_6502::opInvalid<0x12>;
    m_OpcodeHandlers[0x13] = &CPU_6502::opInvalid<0x13>;
    m_OpcodeHandlers[0x14] = &CPU_6502::opInvalid<0x14>;
    m_OpcodeHandlers[0x15] = &CPU_6502::opInvalid<0x15>;
    m_OpcodeHandlers[0x16] = &CPU_6502::opInvalid<0x16>;
    m_OpcodeHandlers[0x17] = &CPU_6502::opInvalid<0x17>;
    m_OpcodeHandlers[0x18] = &CPU_6502::opInvalid<0x18>;
    m_OpcodeHandlers[0x19] = &CPU_6502::opInvalid<0x19>;
    m_OpcodeHandlers[0x1A] = &CPU_6502::opInvalid<0x1A>;
    m_OpcodeHandlers[0x1B] = &CPU_6502::opInvalid<0x1B>;
    m_OpcodeHandlers[0x1C] = &CPU_6502::opInvalid<0x1C>;
    m_OpcodeHandlers[0x1D] = &CPU_6502::opInvalid<0x1D>;
    m_OpcodeHandlers[0x1E] = &CPU_6502::opInvalid<0x1E>;
    m_OpcodeHandlers[0x1F] = &CPU_6502::opInvalid<0x1F>;

    m_OpcodeHandlers[0x20] = &CPU_6502::opInvalid<0x20>;
    m_OpcodeHandlers[0x21] = &CPU_6502::opInvalid<0x21>;
    m_OpcodeHandlers[0x22] = &CPU_6502::opInvalid<0x22>;
    m_OpcodeHandlers[0x23] = &CPU_6502::opInvalid<0x23>;
    m_OpcodeHandlers[0x24] = &CPU_6502::opInvalid<0x24>;
    m_OpcodeHandlers[0x25] = &CPU_6502::opInvalid<0x25>;
    m_OpcodeHandlers[0x26] = &CPU_6502::opInvalid<0x26>;
    m_OpcodeHandlers[0x27] = &CPU_6502::opInvalid<0x27>;
    m_OpcodeHandlers[0x28] = &CPU_6502::opInvalid<0x28>;
    m_OpcodeHandlers[0x29] = &CPU_6502::opInvalid<0x29>;
    m_OpcodeHandlers[0x2A] = &CPU_6502::opInvalid<0x2A>;
    m_OpcodeHandlers[0x2B] = &CPU_6502::opInvalid<0x2B>;
    m_OpcodeHandlers[0x2C] = &CPU_6502::opInvalid<0x2C>;
    m_OpcodeHandlers[0x2D] = &CPU_6502::opInvalid<0x2D>;
    m_OpcodeHandlers[0x2E] = &CPU_6502::opInvalid<0x2E>;
    m_OpcodeHandlers[0x2F] = &CPU_6502::opInvalid<0x2F>;

    m_OpcodeHandlers[0x30] = &CPU_6502::opInvalid<0x30>;
    m_OpcodeHandlers[0x31] = &CPU_6502::opInvalid<0x31>;
    m_OpcodeHandlers[0x32] = &CPU_6502::opInvalid<0x32>;
    m_OpcodeHandlers[0x33] = &CPU_6502::opInvalid<0x33>;
    m_OpcodeHandlers[0x34] = &CPU_6502::opInvalid<0x34>;
    m_OpcodeHandlers[0x35] = &CPU_6502::opInvalid<0x35>;
    m_OpcodeHandlers[0x36] = &CPU_6502::opInvalid<0x36>;
    m_OpcodeHandlers[0x37] = &CPU_6502::opInvalid<0x37>;
    m_OpcodeHandlers[0x38] = &CPU_6502::opInvalid<0x38>;
    m_OpcodeHandlers[0x39] = &CPU_6502::opInvalid<0x39>;
    m_OpcodeHandlers[0x3A] = &CPU_6502::opInvalid<0x3A>;
    m_OpcodeHandlers[0x3B] = &CPU_6502::opInvalid<0x3B>;
    m_OpcodeHandlers[0x3C] = &CPU_6502::opInvalid<0x3C>;
    m_OpcodeHandlers[0x3D] = &CPU_6502::opInvalid<0x3D>;
    m_OpcodeHandlers[0x3E] = &CPU_6502::opInvalid<0x3E>;
    m_OpcodeHandlers[0x3F] = &CPU_6502::opInvalid<0x3F>;

    m_OpcodeHandlers[0x40] = &CPU_6502::opInvalid<0x40>;
    m_OpcodeHandlers[0x41] = &CPU_6502::opInvalid<0x41>;
    m_OpcodeHandlers[0x42] = &CPU_6502::opInvalid<0x42>;
    m_OpcodeHandlers[0x43] = &CPU_6502::opInvalid<0x43>;
    m_OpcodeHandlers[0x44] = &CPU_6502::opInvalid<0x44>;
    m_OpcodeHandlers[0x45] = &CPU_6502::opInvalid<0x45>;
    m_OpcodeHandlers[0x46] = &CPU_6502::opInvalid<0x46>;
    m_OpcodeHandlers[0x47] = &CPU_6502::opInvalid<0x47>;
    m_OpcodeHandlers[0x48] = &CPU_6502::opInvalid<0x48>;
    m_OpcodeHandlers[0x49] = &CPU_6502::opInvalid<0x49>;
    m_OpcodeHandlers[0x4A] = &CPU_6502::opInvalid<0x4A>;
    m_OpcodeHandlers[0x4B] = &CPU_6502::opInvalid<0x4B>;
    m_OpcodeHandlers[0x4C] = &CPU_6502::opInvalid<0x4C>;
    m_OpcodeHandlers[0x4D] = &CPU_6502::opInvalid<0x4D>;
    m_OpcodeHandlers[0x4E] = &CPU_6502::opInvalid<0x4E>;
    m_OpcodeHandlers[0x4F] = &CPU_6502::opInvalid<0x4F>;

    m_OpcodeHandlers[0x50] = &CPU_6502::opInvalid<0x50>;
    m_OpcodeHandlers[0x51] = &CPU_6502::opInvalid<0x51>;
    m_OpcodeHandlers[0x52] = &CPU_6502::opInvalid<0x52>;
    m_OpcodeHandlers[0x53] = &CPU_6502::opInvalid<0x53>;
    m_OpcodeHandlers[0x54] = &CPU_6502::opInvalid<0x54>;
    m_OpcodeHandlers[0x55] = &CPU_6502::opInvalid<0x55>;
    m_OpcodeHandlers[0x56] = &CPU_6502::opInvalid<0x56>;
    m_OpcodeHandlers[0x57] = &CPU_6502::opInvalid<0x57>;
    m_OpcodeHandlers[0x58] = &CPU_6502::opInvalid<0x58>;
    m_OpcodeHandlers[0x59] = &CPU_6502::opInvalid<0x59>;
    m_OpcodeHandlers[0x5A] = &CPU_6502::opInvalid<0x5A>;
    m_OpcodeHandlers[0x5B] = &CPU_6502::opInvalid<0x5B>;
    m_OpcodeHandlers[0x5C] = &CPU_6502::opInvalid<0x5C>;
    m_OpcodeHandlers[0x5D] = &CPU_6502::opInvalid<0x5D>;
    m_OpcodeHandlers[0x5E] = &CPU_6502::opInvalid<0x5E>;
    m_OpcodeHandlers[0x5F] = &CPU_6502::opInvalid<0x5F>;

    m_OpcodeHandlers[0x60] = &CPU_6502::opInvalid<0x60>;
    m_OpcodeHandlers[0x61] = &CPU_6502::opADC_ind_X;
    m_OpcodeHandlers[0x62] = &CPU_6502::opInvalid<0x62>;
    m_OpcodeHandlers[0x63] = &CPU_6502::opInvalid<0x63>;
    m_OpcodeHandlers[0x64] = &CPU_6502::opInvalid<0x64>;
    m_OpcodeHandlers[0x65] = &CPU_6502::opADC_zp;
    m_OpcodeHandlers[0x66] = &CPU_6502::opInvalid<0x66>;
    m_OpcodeHandlers[0x67] = &CPU_6502::opInvalid<0x67>;
    m_OpcodeHandlers[0x68] = &CPU_6502::opInvalid<0x68>;
    m_OpcodeHandlers[0x69] = &CPU_6502::opADC_imm;
    m_OpcodeHandlers[0x6A] = &CPU_6502::opInvalid<0x6A>;
    m_OpcodeHandlers[0x6B] = &CPU_6502::opInvalid<0x6B>;
    m_OpcodeHandlers[0x6C] = &CPU_6502::opInvalid<0x6C>;
    m_OpcodeHandlers[0x6D] = &CPU_6502::opADC_abs;
    m_OpcodeHandlers[0x6E] = &CPU_6502::opInvalid<0x6E>;
    m_OpcodeHandlers[0x6F] = &CPU_6502::opInvalid<0x6F>;

    m_OpcodeHandlers[0x70] = &CPU_6502::opInvalid<0x70>;
    m_OpcodeHandlers[0x71] = &CPU_6502::opADC_ind_Y;
    m_OpcodeHandlers[0x72] = &CPU_6502::opInvalid<0x72>;
    m_OpcodeHandlers[0x73] = &CPU_6502::opInvalid<0x73>;
    m_OpcodeHandlers[0x74] = &CPU_6502::opInvalid<0x74>;
    m_OpcodeHandlers[0x75] = &CPU_6502::opADC_zp_X;
    m_OpcodeHandlers[0x76] = &CPU_6502::opInvalid<0x76>;
    m_OpcodeHandlers[0x77] = &CPU_6502::opInvalid<0x77>;
    m_OpcodeHandlers[0x78] = &CPU_6502::opInvalid<0x78>;
    m_OpcodeHandlers[0x79] = &CPU_6502::opADC_abs_Y;
    m_OpcodeHandlers[0x7A] = &CPU_6502::opInvalid<0x7A>;
    m_OpcodeHandlers[0x7B] = &CPU_6502::opInvalid<0x7B>;
    m_OpcodeHandlers[0x7C] = &CPU_6502::opInvalid<0x7C>;
    m_OpcodeHandlers[0x7D] = &CPU_6502::opADC_abs_X;
    m_OpcodeHandlers[0x7E] = &CPU_6502::opInvalid<0x7E>;
    m_OpcodeHandlers[0x7F] = &CPU_6502::opInvalid<0x7F>;

    m_OpcodeHandlers[0x80] = &CPU_6502::opInvalid<0x80>;
    m_OpcodeHandlers[0x81] = &CPU_6502::opInvalid<0x81>;
    m_OpcodeHandlers[0x82] = &CPU_6502::opInvalid<0x82>;
    m_OpcodeHandlers[0x83] = &CPU_6502::opInvalid<0x83>;
    m_OpcodeHandlers[0x84] = &CPU_6502::opInvalid<0x84>;
    m_OpcodeHandlers[0x85] = &CPU_6502::opInvalid<0x85>;
    m_OpcodeHandlers[0x86] = &CPU_6502::opInvalid<0x86>;
    m_OpcodeHandlers[0x87] = &CPU_6502::opInvalid<0x87>;
    m_OpcodeHandlers[0x88] = &CPU_6502::opInvalid<0x88>;
    m_OpcodeHandlers[0x89] = &CPU_6502::opInvalid<0x89>;
    m_OpcodeHandlers[0x8A] = &CPU_6502::opInvalid<0x8A>;
    m_OpcodeHandlers[0x8B] = &CPU_6502::opInvalid<0x8B>;
    m_OpcodeHandlers[0x8C] = &CPU_6502::opInvalid<0x8C>;
    m_OpcodeHandlers[0x8D] = &CPU_6502::opInvalid<0x8D>;
    m_OpcodeHandlers[0x8E] = &CPU_6502::opInvalid<0x8E>;
    m_OpcodeHandlers[0x8F] = &CPU_6502::opInvalid<0x8F>;

    m_OpcodeHandlers[0x90] = &CPU_6502::opInvalid<0x90>;
    m_OpcodeHandlers[0x91] = &CPU_6502::opInvalid<0x91>;
    m_OpcodeHandlers[0x92] = &CPU_6502::opInvalid<0x92>;
    m_OpcodeHandlers[0x93] = &CPU_6502::opInvalid<0x93>;
    m_OpcodeHandlers[0x94] = &CPU_6502::opInvalid<0x94>;
    m_OpcodeHandlers[0x95] = &CPU_6502::opInvalid<0x95>;
    m_OpcodeHandlers[0x96] = &CPU_6502::opInvalid<0x96>;
    m_OpcodeHandlers[0x97] = &CPU_6502::opInvalid<0x97>;
    m_OpcodeHandlers[0x98] = &CPU_6502::opInvalid<0x98>;
    m_OpcodeHandlers[0x99] = &CPU_6502::opInvalid<0x99>;
    m_OpcodeHandlers[0x9A] = &CPU_6502::opInvalid<0x9A>;
    m_OpcodeHandlers[0x9B] = &CPU_6502::opInvalid<0x9B>;
    m_OpcodeHandlers[0x9C] = &CPU_6502::opInvalid<0x9C>;
    m_OpcodeHandlers[0x9D] = &CPU_6502::opInvalid<0x9D>;
    m_OpcodeHandlers[0x9E] = &CPU_6502::opInvalid<0x9E>;
    m_OpcodeHandlers[0x9F] = &CPU_6502::opInvalid<0x9F>;

    m_OpcodeHandlers[0xA0] = &CPU_6502::opInvalid<0xA0>;
    m_OpcodeHandlers[0xA1] = &CPU_6502::opInvalid<0xA1>;
    m_OpcodeHandlers[0xA2] = &CPU_6502::opInvalid<0xA2>;
    m_OpcodeHandlers[0xA3] = &CPU_6502::opInvalid<0xA3>;
    m_OpcodeHandlers[0xA4] = &CPU_6502::opInvalid<0xA4>;
    m_OpcodeHandlers[0xA5] = &CPU_6502::opInvalid<0xA5>;
    m_OpcodeHandlers[0xA6] = &CPU_6502::opInvalid<0xA6>;
    m_OpcodeHandlers[0xA7] = &CPU_6502::opInvalid<0xA7>;
    m_OpcodeHandlers[0xA8] = &CPU_6502::opInvalid<0xA8>;
    m_OpcodeHandlers[0xA9] = &CPU_6502::opInvalid<0xA9>;
    m_OpcodeHandlers[0xAA] = &CPU_6502::opInvalid<0xAA>;
    m_OpcodeHandlers[0xAB] = &CPU_6502::opInvalid<0xAB>;
    m_OpcodeHandlers[0xAC] = &CPU_6502::opInvalid<0xAC>;
    m_OpcodeHandlers[0xAD] = &CPU_6502::opInvalid<0xAD>;
    m_OpcodeHandlers[0xAE] = &CPU_6502::opInvalid<0xAE>;
    m_OpcodeHandlers[0xAF] = &CPU_6502::opInvalid<0xAF>;

    m_OpcodeHandlers[0xB0] = &CPU_6502::opInvalid<0xB0>;
    m_OpcodeHandlers[0xB1] = &CPU_6502::opInvalid<0xB1>;
    m_OpcodeHandlers[0xB2] = &CPU_6502::opInvalid<0xB2>;
    m_OpcodeHandlers[0xB3] = &CPU_6502::opInvalid<0xB3>;
    m_OpcodeHandlers[0xB4] = &CPU_6502::opInvalid<0xB4>;
    m_OpcodeHandlers[0xB5] = &CPU_6502::opInvalid<0xB5>;
    m_OpcodeHandlers[0xB6] = &CPU_6502::opInvalid<0xB6>;
    m_OpcodeHandlers[0xB7] = &CPU_6502::opInvalid<0xB7>;
    m_OpcodeHandlers[0xB8] = &CPU_6502::opInvalid<0xB8>;
    m_OpcodeHandlers[0xB9] = &CPU_6502::opInvalid<0xB9>;
    m_OpcodeHandlers[0xBA] = &CPU_6502::opInvalid<0xBA>;
    m_OpcodeHandlers[0xBB] = &CPU_6502::opInvalid<0xBB>;
    m_OpcodeHandlers[0xBC] = &CPU_6502::opInvalid<0xBC>;
    m_OpcodeHandlers[0xBD] = &CPU_6502::opInvalid<0xBD>;
    m_OpcodeHandlers[0xBE] = &CPU_6502::opInvalid<0xBE>;
    m_OpcodeHandlers[0xBF] = &CPU_6502::opInvalid<0xBF>;

    m_OpcodeHandlers[0xC0] = &CPU_6502::opInvalid<0xC0>;
    m_OpcodeHandlers[0xC1] = &CPU_6502::opInvalid<0xC1>;
    m_OpcodeHandlers[0xC2] = &CPU_6502::opInvalid<0xC2>;
    m_OpcodeHandlers[0xC3] = &CPU_6502::opInvalid<0xC3>;
    m_OpcodeHandlers[0xC4] = &CPU_6502::opInvalid<0xC4>;
    m_OpcodeHandlers[0xC5] = &CPU_6502::opInvalid<0xC5>;
    m_OpcodeHandlers[0xC6] = &CPU_6502::opInvalid<0xC6>;
    m_OpcodeHandlers[0xC7] = &CPU_6502::opInvalid<0xC7>;
    m_OpcodeHandlers[0xC8] = &CPU_6502::opINY;
    m_OpcodeHandlers[0xC9] = &CPU_6502::opInvalid<0xC9>;
    m_OpcodeHandlers[0xCA] = &CPU_6502::opInvalid<0xCA>;
    m_OpcodeHandlers[0xCB] = &CPU_6502::opInvalid<0xCB>;
    m_OpcodeHandlers[0xCC] = &CPU_6502::opInvalid<0xCC>;
    m_OpcodeHandlers[0xCD] = &CPU_6502::opInvalid<0xCD>;
    m_OpcodeHandlers[0xCE] = &CPU_6502::opInvalid<0xCE>;
    m_OpcodeHandlers[0xCF] = &CPU_6502::opInvalid<0xCF>;

    m_OpcodeHandlers[0xD0] = &CPU_6502::opInvalid<0xD0>;
    m_OpcodeHandlers[0xD1] = &CPU_6502::opInvalid<0xD1>;
    m_OpcodeHandlers[0xD2] = &CPU_6502::opInvalid<0xD2>;
    m_OpcodeHandlers[0xD3] = &CPU_6502::opInvalid<0xD3>;
    m_OpcodeHandlers[0xD4] = &CPU_6502::opInvalid<0xD4>;
    m_OpcodeHandlers[0xD5] = &CPU_6502::opInvalid<0xD5>;
    m_OpcodeHandlers[0xD6] = &CPU_6502::opInvalid<0xD6>;
    m_OpcodeHandlers[0xD7] = &CPU_6502::opInvalid<0xD7>;
    m_OpcodeHandlers[0xD8] = &CPU_6502::opInvalid<0xD8>;
    m_OpcodeHandlers[0xD9] = &CPU_6502::opInvalid<0xD9>;
    m_OpcodeHandlers[0xDA] = &CPU_6502::opInvalid<0xDA>;
    m_OpcodeHandlers[0xDB] = &CPU_6502::opInvalid<0xDB>;
    m_OpcodeHandlers[0xDC] = &CPU_6502::opInvalid<0xDC>;
    m_OpcodeHandlers[0xDD] = &CPU_6502::opInvalid<0xDD>;
    m_OpcodeHandlers[0xDE] = &CPU_6502::opInvalid<0xDE>;
    m_OpcodeHandlers[0xDF] = &CPU_6502::opInvalid<0xDF>;

    m_OpcodeHandlers[0xE0] = &CPU_6502::opInvalid<0xE0>;
    m_OpcodeHandlers[0xE1] = &CPU_6502::opInvalid<0xE1>;
    m_OpcodeHandlers[0xE2] = &CPU_6502::opInvalid<0xE2>;
    m_OpcodeHandlers[0xE3] = &CPU_6502::opInvalid<0xE3>;
    m_OpcodeHandlers[0xE4] = &CPU_6502::opInvalid<0xE4>;
    m_OpcodeHandlers[0xE5] = &CPU_6502::opInvalid<0xE5>;
    m_OpcodeHandlers[0xE6] = &CPU_6502::opInvalid<0xE6>;
    m_OpcodeHandlers[0xE7] = &CPU_6502::opInvalid<0xE7>;
    m_OpcodeHandlers[0xE8] = &CPU_6502::opINX;
    m_OpcodeHandlers[0xE9] = &CPU_6502::opInvalid<0xE9>;
    m_OpcodeHandlers[0xEA] = &CPU_6502::opNOP;
    m_OpcodeHandlers[0xEB] = &CPU_6502::opInvalid<0xEB>;
    m_OpcodeHandlers[0xEC] = &CPU_6502::opInvalid<0xEC>;
    m_OpcodeHandlers[0xED] = &CPU_6502::opInvalid<0xED>;
    m_OpcodeHandlers[0xEE] = &CPU_6502::opInvalid<0xEE>;
    m_OpcodeHandlers[0xEF] = &CPU_6502::opInvalid<0xEF>;

    m_OpcodeHandlers[0xF0] = &CPU_6502::opInvalid<0xF0>;
    m_OpcodeHandlers[0xF1] = &CPU_6502::opInvalid<0xF1>;
    m_OpcodeHandlers[0xF2] = &CPU_6502::opInvalid<0xF2>;
    m_OpcodeHandlers[0xF3] = &CPU_6502::opInvalid<0xF3>;
    m_OpcodeHandlers[0xF4] = &CPU_6502::opInvalid<0xF4>;
    m_OpcodeHandlers[0xF5] = &CPU_6502::opInvalid<0xF5>;
    m_OpcodeHandlers[0xF6] = &CPU_6502::opInvalid<0xF6>;
    m_OpcodeHandlers[0xF7] = &CPU_6502::opInvalid<0xF7>;
    m_OpcodeHandlers[0xF8] = &CPU_6502::opInvalid<0xF8>;
    m_OpcodeHandlers[0xF9] = &CPU_6502::opInvalid<0xF9>;
    m_OpcodeHandlers[0xFA] = &CPU_6502::opInvalid<0xFA>;
    m_OpcodeHandlers[0xFB] = &CPU_6502::opInvalid<0xFB>;
    m_OpcodeHandlers[0xFC] = &CPU_6502::opInvalid<0xFC>;
    m_OpcodeHandlers[0xFD] = &CPU_6502::opInvalid<0xFD>;
    m_OpcodeHandlers[0xFE] = &CPU_6502::opInvalid<0xFE>;
    m_OpcodeHandlers[0xFF] = &CPU_6502::opInvalid<0xFF>;
}

CPU_6502::~CPU_6502() = default;

void CPU_6502::startup() {
    // https://www.nesdev.org/wiki/Init_code

    m_State.pc = readMemory(VEC_RESET);
    m_State.sp = 0xFD;
    m_State.a = 0;
    m_State.x = 0;
    m_State.y = 0;
    m_State.p = 0;
    setRegister(U, true);
}

void CPU_6502::reset() {
    m_State.pc = readMemory(VEC_RESET);
    m_State.sp -= 3;

    // Other registers remain unchanged on reset
}

void CPU_6502::execute() {
    const uint8 opcode = readMemory(m_State.pc++);

    (this->*m_OpcodeHandlers[opcode])();
}

uint8 CPU_6502::readMemory(const uint16 address) {
    m_Cycles++;
    return m_Bus->read(address);
}

void CPU_6502::writeMemory(uint16 address, const uint8 value) {
    m_Cycles++;
    m_Bus->write(address, value);
}

template<unsigned OP>
void CPU_6502::opInvalid() {
    // TODO: Log invalid opcode (and halt execution?)
    throw std::runtime_error("Invalid opcode");
}

void CPU_6502::opNOP() {
    // NOP has a dummy read which takes an extra cycle
    m_Cycles++;
}

void CPU_6502::opADC(uint8 value) {
    const uint8 carry = getRegister(C);
    const uint8 total = m_State.a + value + carry;

    setRegister(C, m_State.a + value + carry > 0xFF);
    setRegister(Z, !total);
    setRegister(V, (total ^ m_State.a) & (total ^ value) & 0x80);
    setRegister(N, total & 0x80);

    m_State.a = total;
}

void CPU_6502::opADC_imm() {
    const uint8 value = readMemory(m_State.pc++);

    opADC(value);
}

void CPU_6502::opADC_zp() {
    const uint16 addr = readMemory(m_State.pc++);
    const uint8 value = readMemory(addr);

    opADC(value);
}

void CPU_6502::opADC_zp_X() {
    const uint16 addr = readMemory(m_State.pc++) + m_State.x & 0xFF;
    const uint8 value = readMemory(addr);

    // Index mode takes an extra internal cycle
    m_Cycles++;

    opADC(value);
}

void CPU_6502::opADC_abs() {
    const uint8 lo = readMemory(m_State.pc++);
    const uint8 hi = readMemory(m_State.pc++);
    const uint8 value = readMemory(hi << 8 | lo);

    opADC(value);
}

void CPU_6502::opADC_abs_X() {
    const uint8 lo = readMemory(m_State.pc++);
    const uint8 hi = readMemory(m_State.pc++);
    const uint8 value = readMemory((hi << 8 | lo) +  m_State.x);

    if (lo + m_State.x > 0xFF) {
        // Crossing a page boundry takes an extra internal cycle
        m_Cycles++;
    }

    opADC(value);
}

void CPU_6502::opADC_abs_Y() {
    const uint8 lo = readMemory(m_State.pc++);
    const uint8 hi = readMemory(m_State.pc++);
    const uint8 value = readMemory((hi << 8 | lo) +  m_State.y);

    if (lo + m_State.y > 0xFF) {
        // Crossing a page boundry takes an extra internal cycle
        m_Cycles++;
    }

    opADC(value);
}

void CPU_6502::opADC_ind_X() {
    uint8 addr = readMemory(m_State.pc++) + m_State.x;
    const uint8 lo = readMemory(addr++);
    const uint8 hi = readMemory(addr);
    const uint8 value = readMemory(hi << 8 | lo);

    // Index mode takes an extra internal cycle
    m_Cycles++;

    opADC(value);
}

void CPU_6502::opADC_ind_Y() {
    uint8 addr = readMemory(m_State.pc++);
    const uint8 lo = readMemory(addr++);
    const uint8 hi = readMemory(addr);
    const uint8 value = readMemory((hi << 8 | lo) + m_State.y);

    if (lo + m_State.y > 0xFF) {
        // Crossing a page boundry takes an extra internal cycle
        m_Cycles++;
    }

    opADC(value);
}

void CPU_6502::opINX() {
    m_State.x++;

    setRegister(N, m_State.x & 0x80);
    setRegister(Z, !m_State.x);

    // implied addressing mode takes an extra cycle
    m_Cycles++;
}

void CPU_6502::opINY() {
    m_State.y++;

    setRegister(N, m_State.y & 0x80);
    setRegister(Z, !m_State.y);

    // implied addressing mode takes an extra cycle
    m_Cycles++;
}
