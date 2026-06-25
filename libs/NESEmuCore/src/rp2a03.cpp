#include "NESEmuCore/rp2a03.hpp"

#include "NESEmuCore/controller.hpp"

using namespace NESEmu;

Controller  dummyController;
Controller& Rp2A03::s_dummyController = dummyController;

void Rp2A03::startup()
{
    m_cpu.startup();
}

void Rp2A03::reset()
{
    m_cpu.reset();
    m_dma.reset();
}

void Rp2A03::execute()
{
    m_dma.active() ? m_dma.execute() : m_cpu.execute();
}

void Rp2A03::attachController(const InputPort port, Controller* controller)
{
    m_controllers[port] = controller;
}

uint8 Rp2A03::onCpuRead(const uint16 address)
{
    switch (static_cast<Registers>(address)) {
        case Registers::kJoypad1:
            return readController(InputPort::Joypad1);
        case Registers::kJoypad2:
            return readController(InputPort::Joypad2);
        default:
            break;
    }

    // Note: Open bus behavior will be incorrect here
    return 0;
}

void Rp2A03::onCpuWrite(uint16 address, uint8 data)
{
    switch (static_cast<Registers>(address)) {
        case Registers::kOamDmaTransfer:
            m_dma.requestOamDma(data);
            break;
        case Registers::kJoypadCtrl:
            writeController(data);
            break;
        default:
            break;
    }
}

void Rp2A03::writeController(const uint8 data)
{
    const bool poll = data & 1;
    for (auto controller : m_controllers) {
        if (controller != nullptr) {
            poll ? controller->startPoll() : controller->stopPoll();
        }
    }
}

bool Rp2A03::readController(const InputPort port)
{
    auto controller = m_controllers.at(port);
    if (controller != nullptr) {

        return controller->readControllerInputBit();
    }
    return false;
}
