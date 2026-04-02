#include "NESEmuCore/cpu_6502.hpp"

CPU_6502::CPU_6502() {
    m_OpcodeHandlers[0x00] = &CPU_6502::opInvalid<0x00>;
    m_OpcodeHandlers[0x01] = &CPU_6502::opInvalid<0x01>;
    m_OpcodeHandlers[0x02] = &CPU_6502::opInvalid<0x02>;
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
    setRegister(U);
}

void CPU_6502::reset() {
    m_State.pc = readMemory(VEC_RESET);
    m_State.sp -= 3;

    // Other registers remain unchanged on reset
}

void CPU_6502::execute() {
    const t_uint8 opcode = readMemory(m_State.pc++);

    (this->*m_OpcodeHandlers[opcode])();
}

t_uint8 CPU_6502::readMemory(const t_uint16 address) const {
    return m_Memory[address];
}

void CPU_6502::writeMemory(t_uint16 address, const t_uint8 value) {
    m_Memory[address] = value;
}

template<unsigned OP>
void CPU_6502::opInvalid() {
    // TODO: Log invalid opcode (and halt execution?)
}

void CPU_6502::opNOP() {
}
