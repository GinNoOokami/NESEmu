#include "NESEmuCore/bus.hpp"

#include "NESEmuCore/memory.hpp"

using namespace NESEmu;

uint8 Bus::read(const uint16 address)
{
    uint8 data = m_openBusData;

    /*switch (address & ADDRESS_MASK) {
    case MEMORY_ENABLE_MASK:
        data = m_openBusData = m_memory.read(address);
        break;
    default:
        break;
    }*/

    if (m_memory.canRead(address)) {
        data = m_openBusData = m_memory.read(address);
    }

    return data;
}

void Bus::write(const uint16 address, const uint8 data)
{
    /*switch (address & ADDRESS_MASK) {
    case MEMORY_ENABLE_MASK:
        m_memory.write(address, data);
        m_openBusData = data;
        break;
    default:
        break;
    }*/

    if (m_memory.canWrite(address)) {
        m_memory.write(address, data);
        m_openBusData = data;
    }
}
