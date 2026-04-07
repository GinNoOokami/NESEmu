#ifndef NESEMU_CPU_6502_HPP
#define NESEMU_CPU_6502_HPP

#include "emu_types.hpp"

namespace NESEmu {
    class Bus;

    class CPU_6502 {
        static constexpr uint16 VEC_NMI   = 0xFFFA;
        static constexpr uint16 VEC_RESET = 0xFFFC;
        static constexpr uint16 VEC_IRQ   = 0xFFFE;

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

        struct CPUState_6502 {
            uint16    pc;
            uint16    sp;
            uint8     a;
            uint8     x;
            uint8     y;
            uint8     p;
        };

        CPU_6502(Bus* bus);
        ~CPU_6502();

        void startup();
        void reset();
        void execute();

        [[nodiscard]]   const CPUState_6502&  state() const { return m_State; }
                        void            state(const CPUState_6502& state) { m_State = state; }

        [[nodiscard]]   uint8           readMemory(uint16 address) const;
                        void            writeMemory(uint16 address, uint8 value);

        [[nodiscard]]   uint32          cycles() const { return m_Cycles; };

    private:
        typedef void (CPU_6502::*OpcodeHandler)();

        [[nodiscard]]   bool            getRegister(const RegisterFlag flag) const          { return m_State.p & flag;                          }
        void                            setRegister(const RegisterFlag flag, bool value)    { value ? m_State.p |= flag : m_State.p &= ~flag;   }

        // Opcodes
        template<unsigned OP>   void    opInvalid();
                                void    opNOP();
                                void    opINX();
                                void    opINY();

        CPUState_6502   m_State {};
        OpcodeHandler   m_OpcodeHandlers[256] { nullptr };

        uint32          m_Cycles { 0 };

        Bus*            m_Bus;
    };

    inline bool operator==(const CPU_6502::CPUState_6502& lhs, const CPU_6502::CPUState_6502& rhs) {
        return lhs.pc == rhs.pc &&
               lhs.sp == rhs.sp &&
               lhs.a  == rhs.a  &&
               lhs.x  == rhs.x  &&
               lhs.y  == rhs.y  &&
               lhs.p  == rhs.p;
    }

    inline bool operator!=(const CPU_6502::CPUState_6502& lhs, const CPU_6502::CPUState_6502& rhs) {
        return !(lhs == rhs);
    }
}

#endif //NESEMU_CPU_6502_HPP
