#ifndef NESEMU_SYSTEM_HPP
#define NESEMU_SYSTEM_HPP

#include "bus.hpp"
#include "clock.hpp"
#include "interrupt_lines.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "rp2a03.hpp"

#include <memory>

namespace NESEmu {
class Cartridge;
class MapperNRom;

class System {
public:
    static constexpr uint32 kMasterClockSpeedHz     = 21477272;
    static constexpr uint32 kMasterClockFrameCycles = 357368;

    System();

    void startup(const Cartridge& cartridge);
    void reset();
    void runFrame();
    void shutdown();

    void attachController(InputPort port, Controller* controller);

    // TODO: Handle one dot less every other PPU frame when rendering is enabled
    [[nodiscard]] constexpr uint32 targetMasterFrameCycles() const { return kMasterClockFrameCycles; }

    [[nodiscard]] const Clock& clock() const { return m_clock; }
    [[nodiscard]] bool         cartridgeLoaded() const { return mCartridgeLoaded; }
    [[nodiscard]] FrameBuffer  frameBuffer() const { return m_ppu.frameBuffer(); }

private:
    Clock          m_clock{};
    WorkRam        m_workRam{};
    CiRam          m_ciRam{};
    InterruptLines m_interruptLines{};
    MainBus        m_mainBus{};
    PpuBus         m_ppuBus;
    Rp2A03         m_cpu;
    Ppu            m_ppu;

    std::unique_ptr<MapperNRom> m_mapper;

    bool mCartridgeLoaded{};
};
}

#endif //NESEMU_SYSTEM_HPP
