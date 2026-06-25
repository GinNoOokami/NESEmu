#ifndef NESEMU_CONTROLLER_HPP
#define NESEMU_CONTROLLER_HPP

#include "emu_types.hpp"

namespace NESEmu {
class Controller {
public:
    void reset();
    void startPoll();
    void stopPoll();

    [[nodiscard]] bool readControllerInputBit();

    void simulatePress(InputButtons button);
    void simulateRelease(InputButtons button);

    [[nodiscard]] bool  strobing() const { return m_strobing; }
    [[nodiscard]] uint8 latchedState() const { return m_latchedState; }
    [[nodiscard]] uint8 simulatedState() const { return m_simulatedState; }

private:
    uint8 m_simulatedState{};
    uint8 m_latchedState{};
    bool  m_strobing{};
};
}

#endif //NESEMU_CONTROLLER_HPP
