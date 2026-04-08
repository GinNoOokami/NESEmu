#ifndef NESEMU_DEBUG_FORMAT_HPP
#define NESEMU_DEBUG_FORMAT_HPP

#include <string>

#include "cpu6502.hpp"

namespace NESEmu {
    template<typename T>
    std::string to_register(T value) {
        std::ostringstream oss;
        oss << "$"
            << std::uppercase
            << std::hex
            << std::setw(sizeof(T) * 2)
            << std::setfill('0')
            << static_cast<uint32_t>(value);
        return oss.str();
    }

    inline std::string format_flags(uint8_t p) {
        return std::string{
            p & Cpu6502::RegisterFlag::N ? 'N' : '.',
            p & Cpu6502::RegisterFlag::V ? 'V' : '.',
            '-',
            p & Cpu6502::RegisterFlag::B ? 'B' : '.',
            p & Cpu6502::RegisterFlag::D ? 'D' : '.',
            p & Cpu6502::RegisterFlag::I ? 'I' : '.',
            p & Cpu6502::RegisterFlag::Z ? 'Z' : '.',
            p & Cpu6502::RegisterFlag::C ? 'C' : '.'
        };
    }

    inline std::string cpu_state_to_string(const Cpu6502::State& state) {
        std::ostringstream oss;
        oss << "PC: " << to_register(state.pc)
            << " SP: " << to_register(state.sp)
            << " A: "  << to_register(state.a)
            << " X: "  << to_register(state.x)
            << " Y: "  << to_register(state.y)
            << " P: [" << format_flags(state.p) << "]";
        return oss.str();
    }

    inline std::ostream &operator<<(std::ostream &os, const Cpu6502::State& value) {
        os << cpu_state_to_string(value);
        return os;
    }
}

#endif //NESEMU_DEBUG_FORMAT_HPP
