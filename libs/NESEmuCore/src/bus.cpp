#include "NESEmuCore/bus.hpp"

Bus::Bus() = default;
Bus::~Bus() = default;

void Bus::write(t_uint16 address, t_uint8 data) {
}

t_uint8 Bus::read(t_uint16 address) {
    return 0x00;
}
