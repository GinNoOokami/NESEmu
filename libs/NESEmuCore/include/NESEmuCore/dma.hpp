#ifndef NESEMU_DMA_HPP
#define NESEMU_DMA_HPP

#include "NESEmuCore/emu_types.hpp"

namespace NESEmu {
class Clock;
class MainBus;

class DmaController {

public:
    explicit DmaController(Clock& clock, MainBus& bus) : m_clock(clock), m_bus(bus) {}

    void execute();
    void requestOamDma(uint8 page);

    [[nodiscard]] bool active() const { return m_oamDmaActive; }

private:
    static constexpr uint16 kOamDmaDataRegister = 0x2004;

    uint16 m_oamDmaIndex{};
    uint16 m_oamDmaAddress{};
    bool   m_oamDmaActive{};

    Clock&   m_clock;
    MainBus& m_bus;
};
}

#endif //NESEMU_DMA_HPP
