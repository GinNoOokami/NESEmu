#ifndef NESEMU_CPU_6502_HPP
#define NESEMU_CPU_6502_HPP

#include "emu_types.hpp"

struct CPUState_6502 {
    t_uint16 pc;
    t_uint16 sp;
    t_uint8 s;
    t_uint8 a;
    t_uint8 x;
    t_uint8 y;
};

class CPU_6502 {
public:
    CPU_6502();
    CPU_6502(const CPUState_6502& state);

    ~CPU_6502();

    [[nodiscard]] const CPUState_6502& state() const { return m_State; }

    [[nodiscard]] t_uint16 pc() const { return m_State.pc; }
    [[nodiscard]] t_uint16 sp() const { return m_State.sp; }

private:
    CPUState_6502 m_State {};
};

#endif //NESEMU_CPU_6502_HPP
