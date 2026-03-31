#include "../include/NESEmuCore/cpu_6502.hpp"

CPU_6502::CPU_6502() = default;

CPU_6502::CPU_6502(const CPUState_6502& state) :
    m_State(state)
{
}

CPU_6502::~CPU_6502() = default;
