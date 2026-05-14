#include "NESEmuCore/dma.hpp"

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/clock.hpp"

using namespace NESEmu;

void DmaController::reset()
{
    m_oamDmaIndex   = 0;
    m_oamDmaAddress = 0;
    m_oamDmaActive  = false;
}

void DmaController::execute()
{
    // TODO: There is an extra idle cycle on odd cpu cycles
    auto data = m_bus.read(m_oamDmaAddress);
    m_bus.write(kOamDmaDataRegister, data);

    m_clock.advanceCpuCycles(2);

    m_oamDmaAddress++;
    m_oamDmaIndex++;

    m_oamDmaActive = m_oamDmaIndex < 256;
}

void DmaController::requestOamDma(uint8 page)
{
    // DMA transfer has a dummy read during initial request
    m_clock.advanceCpuCycles(1);

    m_oamDmaIndex   = 0;
    m_oamDmaAddress = page << 8;
    m_oamDmaActive  = true;
}
