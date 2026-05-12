#ifndef NESEMU_CLOCK_HPP
#define NESEMU_CLOCK_HPP

#include "NESEmuCore/emu_types.hpp"

namespace NESEmu {
class Clock {
public:
    void reset() { m_masterCycles = 0; }

    constexpr void advanceCpuCycles(const uint8 cycles) { m_masterCycles += cycles * kCpuClockDivider; }

    [[nodiscard]] constexpr uint64 masterCycles() const { return m_masterCycles; }
    [[nodiscard]] constexpr uint64 cpuCycles() const { return m_masterCycles / kCpuClockDivider; }
    [[nodiscard]] constexpr uint64 ppuCycles() const { return m_masterCycles / kPpuClockDivider; }

private:
    static constexpr uint64 kCpuClockDivider = 12;
    static constexpr uint64 kPpuClockDivider = 4;

    uint64 m_masterCycles{};
};
}

#endif //NESEMU_CLOCK_HPP
