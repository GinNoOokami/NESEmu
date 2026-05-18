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
}

#endif //NESEMU_EMU_TYPES_HPP
