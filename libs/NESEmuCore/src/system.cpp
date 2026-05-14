#include "NESEmuCore/system.hpp"

NESEmu::System::System()
    : m_cpu(m_clock, m_mainBus, m_interruptLines),
      m_ppu(m_ppuBus, m_interruptLines) {}

void NESEmu::System::startup(const Cartridge& cartridge)
{
    m_mapper = cartridge.loadMapper();

    m_mainBus.attachRegion(AddressRegion::WorkRam, m_workRam);
    m_mainBus.attachRegion(AddressRegion::Ppu, m_ppu);
    m_mainBus.attachRegion(AddressRegion::ApuIo, m_cpu);
    m_mainBus.attachRegion(AddressRegion::Cartridge0, *m_mapper);
    m_mainBus.attachRegion(AddressRegion::Cartridge1, *m_mapper);
    m_mainBus.attachRegion(AddressRegion::Cartridge2, *m_mapper);
    m_mainBus.attachRegion(AddressRegion::Cartridge3, *m_mapper);
    m_mainBus.attachRegion(AddressRegion::Cartridge4, *m_mapper);

    m_cpu.startup();
    m_ppu.startup();
}

void NESEmu::System::reset()
{
    m_clock.reset();
    m_interruptLines.reset();
    m_cpu.reset();
    m_ppu.reset();
}

void NESEmu::System::runFrame()
{
    const uint64 targetFrameCycles = m_clock.masterCycles() + targetMasterFrameCycles();
    while (m_clock.masterCycles() < targetFrameCycles) {
        m_cpu.execute();

        // Now catch up the rest of the system devices
        m_ppu.executeUntil(m_clock.ppuCycles());
    }
}
