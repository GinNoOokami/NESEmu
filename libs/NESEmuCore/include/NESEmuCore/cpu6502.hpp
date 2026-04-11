#ifndef NESEMU_CPU_6502_HPP
#define NESEMU_CPU_6502_HPP

#include "emu_types.hpp"

namespace NESEmu {
class Bus;

class Cpu6502 {
    static constexpr uint16 kVecNmi   = 0xFFFA;
    static constexpr uint16 kVecReset = 0xFFFC;
    static constexpr uint16 kVecIrq   = 0xFFFE;

    static constexpr uint16 kStackAddr = 0x0100;

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
        uint8  sp;
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

    inline void  pushStack(uint8 value);
    inline uint8 popStack();

    // Addressing modes
    inline void                 addressModeImplied();
    inline uint8                addressModeImmediate();
    inline uint8                addressModeRelative();
    inline uint8                addressModeZeroPage();
    inline uint8                addressModeZeroPageX();
    inline uint8                addressModeZeroPageY();
    inline uint8                addressModeAbsolute();
    template <bool WRITE> uint8 addressModeAbsoluteX();
    template <bool WRITE> uint8 addressModeAbsoluteY();
    inline uint8                addressModeIndirectX();
    template <bool WRITE> uint8 addressModeIndirectY();

    // Opcodes
    template <unsigned OP> void opInvalid();
    void                        opNOP();
    inline void                 opADC();
    void                        opADC_imm();
    void                        opADC_zp();
    void                        opADC_zp_X();
    void                        opADC_abs();
    void                        opADC_abs_X();
    void                        opADC_abs_Y();
    void                        opADC_ind_X();
    void                        opADC_ind_Y();
    inline void                 opAND();
    void                        opAND_imm();
    void                        opAND_zp();
    void                        opAND_zp_X();
    void                        opAND_abs();
    void                        opAND_abs_X();
    void                        opAND_abs_Y();
    void                        opAND_ind_X();
    void                        opAND_ind_Y();
    inline void                 opASL();
    void                        opASL_acc();
    void                        opASL_zp();
    void                        opASL_zp_X();
    void                        opASL_abs();
    void                        opASL_abs_X();
    inline void                 opBRC(bool branch);
    void                        opBCC();
    void                        opBCS();
    void                        opBEQ();
    inline void                 opBIT();
    void                        opBIT_zp();
    void                        opBIT_abs();
    void                        opBMI();
    void                        opBNE();
    void                        opBPL();
    void                        opBVC();
    void                        opBVS();
    void                        opCLC();
    void                        opCLD();
    void                        opCLI();
    void                        opCLV();
    inline void                 opCMP();
    void                        opCMP_imm();
    void                        opCMP_zp();
    void                        opCMP_zp_X();
    void                        opCMP_abs();
    void                        opCMP_abs_X();
    void                        opCMP_abs_Y();
    void                        opCMP_ind_X();
    void                        opCMP_ind_Y();
    inline void                 opCPX();
    void                        opCPX_imm();
    void                        opCPX_zp();
    void                        opCPX_abs();
    inline void                 opCPY();
    void                        opCPY_imm();
    void                        opCPY_zp();
    void                        opCPY_abs();
    inline void                 opDEC();
    void                        opDEC_zp();
    void                        opDEC_zp_X();
    void                        opDEC_abs();
    void                        opDEC_abs_X();
    void                        opDEX();
    void                        opDEY();
    inline void                 opEOR();
    void                        opEOR_imm();
    void                        opEOR_zp();
    void                        opEOR_zp_X();
    void                        opEOR_abs();
    void                        opEOR_abs_X();
    void                        opEOR_abs_Y();
    void                        opEOR_ind_X();
    void                        opEOR_ind_Y();
    inline void                 opINC();
    void                        opINC_zp();
    void                        opINC_zp_X();
    void                        opINC_abs();
    void                        opINC_abs_X();
    void                        opINX();
    void                        opINY();
    void                        opJMP_abs();
    void                        opJMP_ind();
    void                        opJSR();
    inline void                 opLDA();
    void                        opLDA_imm();
    void                        opLDA_zp();
    void                        opLDA_zp_X();
    void                        opLDA_abs();
    void                        opLDA_abs_X();
    void                        opLDA_abs_Y();
    void                        opLDA_ind_X();
    void                        opLDA_ind_Y();
    inline void                 opLDX();
    void                        opLDX_imm();
    void                        opLDX_zp();
    void                        opLDX_zp_Y();
    void                        opLDX_abs();
    void                        opLDX_abs_Y();
    inline void                 opLDY();
    void                        opLDY_imm();
    void                        opLDY_zp();
    void                        opLDY_zp_X();
    void                        opLDY_abs();
    void                        opLDY_abs_X();
    void                        opRTS();

    OpcodeHandler m_opcodeHandlers[256]{ nullptr };

    // Observable state
    State m_state{};

    // Internal state
    uint16 m_address = 0;
    uint8  m_data    = 0;
    uint32 m_cycles  = 0;

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
