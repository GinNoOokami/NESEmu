#include "NESEmuCore/bus.hpp"

using namespace NESEmu;

Bus::Bus() = default;
Bus::~Bus() = default;

void Bus::write(uint16 address, uint8 data) {
}

uint8 Bus::read(uint16 address) {
    return 0x00;
}
