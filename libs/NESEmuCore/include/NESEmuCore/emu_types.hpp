#ifndef NESEMU_EMU_TYPES_HPP
#define NESEMU_EMU_TYPES_HPP

#include <array>
#include <cstdint>

namespace NESEmu {
static constexpr int kScreenDotWidth  = 256;
static constexpr int kScreenDotHeight = 240;

using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using sint8  = std::int8_t;
using sint16 = std::int16_t;
using sint32 = std::int32_t;

using PaletteIndex = uint8;
using FrameBuffer  = std::array<PaletteIndex, kScreenDotWidth * kScreenDotHeight>;

enum InputPort {
    Joypad1,
    Joypad2,
    kMaxInputPorts
};

enum class InputButtons {
    A = 1 << 0,
    B = 1 << 1,
    Select = 1 << 2,
    Start = 1 << 3,
    Up = 1 << 4,
    Down = 1 << 5,
    Left = 1 << 6,
    Right = 1 << 7,
};
}

#endif //NESEMU_EMU_TYPES_HPP
