#ifndef NESEMU_SYSTEM_HPP
#define NESEMU_SYSTEM_HPP

#include "bus.hpp"
#include "clock.hpp"
#include "interrupt_lines.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "rp2a03.hpp"

namespace NESEmu {
class System {
public:
    System();

    void startup(const Cartridge& cartridge);
    void reset();
    void runFrame();

    [[nodiscard]] const Clock& clock() const { return m_clock; }

private:
    static constexpr uint32 kMasterClockFrameCycles = 357368;

    // TODO: Handle one dot less every other PPU frame when rendering is enabled
    [[nodiscard]] constexpr uint32 targetMasterFrameCycles() const { return kMasterClockFrameCycles; }

    Clock          m_clock{};
    WorkRam        m_workRam{};
    MainBus        m_mainBus{};
    PpuBus         m_ppuBus{};
    InterruptLines m_interruptLines{};
    Rp2A03         m_cpu;
    Ppu            m_ppu;

    std::unique_ptr<MapperNRom> m_mapper;
};
}

#endif //NESEMU_SYSTEM_HPP
