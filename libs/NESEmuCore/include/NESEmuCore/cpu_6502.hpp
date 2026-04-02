#ifndef NESEMU_CPU_6502_HPP
#define NESEMU_CPU_6502_HPP

#include "emu_types.hpp"

class CPU_6502 {
    static constexpr t_uint16 VEC_NMI   = 0xFFFA;
    static constexpr t_uint16 VEC_RESET = 0xFFFC;
    static constexpr t_uint16 VEC_IRQ   = 0xFFFE;

    enum RegisterFlag {
        C = 1 << 0, // Carry
        Z = 1 << 1, // Zero
        I = 1 << 2, // Interrupt disable
        D = 1 << 3, // Decimal (disabled on NES)
        B = 1 << 4, // B flag (internal)
        U = 1 << 5, // Unused (always reads 1)
        O = 1 << 6, // Overflow
        N = 1 << 7, // Negative
    };

public:
    struct CPUState_6502 {
        t_uint16    pc;
        t_uint16    sp;
        t_uint8     p;
        t_uint8     a;
        t_uint8     x;
        t_uint8     y;
    };

    CPU_6502();
    ~CPU_6502();

    void startup();
    void reset();
    void execute();

    [[nodiscard]] const CPUState_6502&      state() const { return m_State; }

private:
    typedef void (CPU_6502::*OpcodeHandler)();

    [[nodiscard]]               t_uint8     readMemory(t_uint16 address) const;
                                void        writeMemory(t_uint16 address, t_uint8 value);

    [[nodiscard]]               bool        getRegister(const RegisterFlag flag) const  { return m_State.p & flag;  }
                                void        setRegister(const RegisterFlag flag)        { m_State.p |= flag;        }
                                void        unsetRegister(const RegisterFlag flag)      { m_State.p &= ~flag;       }

    // Opcodes
    template<unsigned OP>       void        opInvalid();
                                void        opNOP();

    CPUState_6502   m_State {};
    OpcodeHandler   m_OpcodeHandlers[256] { nullptr };

    // For now, just embed memory inside the CPU to get something working
    t_uint8         m_Memory[0xFFFF] {};
};

#endif //NESEMU_CPU_6502_HPP
