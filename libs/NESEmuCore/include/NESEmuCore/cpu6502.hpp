#ifndef NESEMU_CPU_6502_HPP
#define NESEMU_CPU_6502_HPP

#include "emu_types.hpp"

namespace NESEmu {
class Bus;

class Cpu6502 {
    static constexpr uint16 kVecNmi   = 0xFFFA;
    static constexpr uint16 kVecReset = 0xFFFC;
    static constexpr uint16 kVecIrq   = 0xFFFE;

public:
    enum RegisterFlag {
        C = 1 << 0, // Carry
        Z = 1 << 1, // Zero
        I = 1 << 2, // Interrupt disable
        D = 1 << 3, // Decimal (disabled on NES)
        B = 1 << 4, // B flag (internal)
        U = 1 << 5, // Unused (always reads 1)
        V = 1 << 6, // Overflow
        N = 1 << 7, // Negative
    };

    struct State {
        uint16 pc;
        uint16 sp;
        uint8  a;
        uint8  x;
        uint8  y;
        uint8  p;
    };

    explicit Cpu6502(Bus& bus);
    ~Cpu6502();

    void startup();
    void reset();
    void execute();

    [[nodiscard]] const State& state() const { return m_state; }
    void                       state(const State& state) { m_state = state; }


    [[nodiscard]] uint32 cycles() const { return m_cycles; }

private:
    typedef void (Cpu6502::*OpcodeHandler)();

    [[nodiscard]] inline uint8 readMemory(uint16 address);
    inline void                writeMemory(uint16 address, uint8 value);

    [[nodiscard]] bool getRegister(const RegisterFlag flag) const { return m_state.p & flag; }
    void               setRegister(const RegisterFlag flag, bool value) { m_state.p = (m_state.p & ~flag) | (-static_cast<uint8>(value) & flag); }

    // Opcodes
    template <unsigned OP> void opInvalid();
    void                        opNOP();
    inline void                 opADC(uint8 value);
    void                        opADC_imm();
    void                        opADC_zp();
    void                        opADC_zp_X();
    void                        opADC_abs();
    void                        opADC_abs_X();
    void                        opADC_abs_Y();
    void                        opADC_ind_X();
    void                        opADC_ind_Y();
    void                        opINX();
    void                        opINY();

    State         m_state{};
    OpcodeHandler m_opcodeHandlers[256]{ nullptr };

    uint32 m_cycles = 0;

    Bus& m_bus;
};

inline bool operator==(const Cpu6502::State& lhs, const Cpu6502::State& rhs)
{
    return lhs.pc == rhs.pc &&
           lhs.sp == rhs.sp &&
           lhs.a == rhs.a &&
           lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.p == rhs.p;
}

inline bool operator!=(const Cpu6502::State& lhs, const Cpu6502::State& rhs)
{
    return !(lhs == rhs);
}
}

#endif //NESEMU_CPU_6502_HPP
