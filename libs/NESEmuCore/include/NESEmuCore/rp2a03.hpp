#ifndef NESEMU_RP2A03_HPP
#define NESEMU_RP2A03_HPP

#include "cpu6502.hpp"
#include "dma.hpp"
#include "emu_types.hpp"

namespace NESEmu {
class Clock;
class Controller;

class Rp2A03 {
public:
    enum class Registers : uint16 {
        kOamDmaTransfer = 0x4014,
        kJoypadCtrl = 0x4016,
        kJoypad1 = 0x4016,
        kJoypad2 = 0x4017,
    };

    Rp2A03(Clock& clock, MainBus& bus, InterruptLines& irq)
        : m_clock(clock), m_cpu(clock, bus, irq), m_dma(clock, bus) {}

    void startup();
    void reset();
    void execute();

    void attachController(InputPort port, Controller* controller);

    [[nodiscard]] uint8 onCpuRead(uint16 address);
    void                onCpuWrite(uint16 address, uint8 data);

private:
    void               writeController(uint8 data);
    [[nodiscard]] bool readController(InputPort port);


    Clock& m_clock;

    Cpu6502       m_cpu;
    DmaController m_dma;

    std::array<Controller*, kMaxInputPorts> m_controllers{};

    static Controller& s_dummyController;
};

inline bool operator==(const uint16 lhs, Rp2A03::Registers rhs)
{
    return lhs == static_cast<uint16>(rhs);
}
}

#endif //NESEMU_RP2A03_HPP
