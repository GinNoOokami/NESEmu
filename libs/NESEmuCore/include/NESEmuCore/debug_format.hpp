#ifndef NESEMU_DEBUG_FORMAT_HPP
#define NESEMU_DEBUG_FORMAT_HPP

#include <string>

#include "cpu6502.hpp"

namespace NESEmu {
template <typename T>
std::string toRegister(T value)
{
    std::ostringstream oss;
    oss << "$"
        << std::uppercase
        << std::hex
        << std::setw(sizeof(T) * 2)
        << std::setfill('0')
        << static_cast<uint32_t>(value);
    return oss.str();
}

inline std::string formatFlags(uint8_t p)
{
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

inline std::string cpuStateToString(const Cpu6502::State& state)
{
    std::ostringstream oss;
    oss << "PC: " << toRegister(state.pc)
        << " SP: " << toRegister(state.sp)
        << " A: " << toRegister(state.a)
        << " X: " << toRegister(state.x)
        << " Y: " << toRegister(state.y)
        << " P: [" << formatFlags(state.p) << "]";
    return oss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Cpu6502::State& value)
{
    os << cpuStateToString(value);
    return os;
}
}

#endif //NESEMU_DEBUG_FORMAT_HPP
