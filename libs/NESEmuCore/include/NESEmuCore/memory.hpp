#ifndef NESEMU_MEMORY_HPP
#define NESEMU_MEMORY_HPP

#include "emu_types.hpp"

#include <array>

namespace NESEmu {
class WorkRam {
public:
    [[nodiscard]] uint8 read(const uint16 address) const { return m_memory[address & kAddressMirrorMask]; }
    void                write(const uint16 address, const uint8 data) { m_memory[address & kAddressMirrorMask] = data; }

private:
    static constexpr uint16 kAddressMirrorMask = 0b0000011111111111;

    // The NES cpu has 2KB of memory, with 3 mirrored sections repeated from $800-$1FFF
    std::array<uint8, 0x800> m_memory{};
};
}

#endif //NESEMU_MEMORY_HPP
