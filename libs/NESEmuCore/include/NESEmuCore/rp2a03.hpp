#ifndef NESEMU_RP2A03_HPP
#define NESEMU_RP2A03_HPP

#include "cpu6502.hpp"
#include "dma.hpp"
#include "emu_types.hpp"

namespace NESEmu {
class Clock;

class Rp2A03 {
public:
    enum class Registers : uint16 {
        kOamDmaTransfer = 0x4014,
    };

    Rp2A03(Clock& clock, MainBus& bus, InterruptLines& irq) : m_clock(clock), m_cpu(clock, bus, irq), m_dma(clock, bus) {}

    void startup();
    void reset();
    void execute();

    [[nodiscard]] uint8 read(uint16 address);
    void                write(uint16 address, uint8 data);

private:
    Clock& m_clock;

    Cpu6502       m_cpu;
    DmaController m_dma;
};

inline bool operator==(const uint16 lhs, Rp2A03::Registers rhs)
{
    return lhs == static_cast<uint16>(rhs);
}
}

#endif //NESEMU_RP2A03_HPP
