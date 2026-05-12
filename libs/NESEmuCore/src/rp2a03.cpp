#include "NESEmuCore/rp2a03.hpp"

using namespace NESEmu;

void Rp2A03::startup()
{
    m_cpu.startup();
}

void Rp2A03::reset()
{
    m_cpu.reset();
}

void Rp2A03::execute()
{
    m_dma.active() ? m_dma.execute() : m_cpu.execute();
}

uint8 Rp2A03::read(const uint16 address)
{
    // TODO: Apu/IO registers
    // Note: Open bus behavior will be incorrect here
    return 0;
}

void Rp2A03::write(uint16 address, uint8 data)
{
    if (address == Registers::kOamDmaTransfer) {
        m_dma.requestOamDma(data);
    }
}
