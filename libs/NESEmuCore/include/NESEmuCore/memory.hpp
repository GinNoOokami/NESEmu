#ifndef NESEMU_MEMORY_HPP
#define NESEMU_MEMORY_HPP

#include "emu_types.hpp"
#include "bus_mappable.hpp"

#include <array>

namespace NESEmu {
class InternalRam : public BusMappable<InternalRam> {
    friend BusMappable;

    static constexpr uint16 ADDRESS_MASK        = 0b1111100000000000;
    static constexpr uint16 ADDRESS_ENABLE_MASK = 0b0001100000000000;
    static constexpr uint16 ADDRESS_MIRROR_MASK = 0b0000011111111111;

public:
    InternalRam()  = default;
    ~InternalRam() = default;

protected:
    [[nodiscard]] static bool isReadEnabled(const uint16 address) { return (address & ADDRESS_MASK) == ADDRESS_ENABLE_MASK; }
    [[nodiscard]] static bool isWriteEnabled(const uint16 address) { return (address & ADDRESS_MASK) == ADDRESS_ENABLE_MASK; }
    [[nodiscard]] uint8       readBus(const uint16 address) const { return m_memory[address & ADDRESS_MIRROR_MASK]; }
    void                      writeBus(const uint16 address, const uint8 data) { m_memory[address & ADDRESS_MIRROR_MASK] = data; }

private:
    // The NES cpu has 2KB of memory, with 3 mirrored sections repeated from $800-$1FFF
    std::array<uint8, 0x800> m_memory{};
};
}

#endif //NESEMU_MEMORY_HPP
