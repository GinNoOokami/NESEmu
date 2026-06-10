#include "NESEmuCore/controller.hpp"

using namespace NESEmu;


void Controller::reset()
{
    m_strobing       = false;
    m_simulatedState = 0;
    m_latchedState   = 0;
}

void Controller::startPoll()
{
    m_strobing = true;
}

void Controller::stopPoll()
{
    m_strobing     = false;
    m_latchedState = m_simulatedState;
}

bool Controller::readControllerInputBit()
{
    if (m_strobing) {
        m_latchedState = m_simulatedState;
    }

    const bool bit = m_latchedState & 0x01;

    m_latchedState >>= 1;

    // Official NES controllers return 1 after initial 8 bits
    m_latchedState |= 0x80;

    return bit;
}

void Controller::simulatePress(InputButtons button)
{
    m_simulatedState |= static_cast<uint8>(button);
}

void Controller::simulateRelease(InputButtons button)
{
    m_simulatedState &= ~static_cast<uint8>(button);
}
