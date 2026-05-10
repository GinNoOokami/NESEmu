#include "NESEmuCore/bus.hpp"

using namespace NESEmu;

void MainBus::write(const uint16 address, const uint8 data)
{
    auto& handler = getHandler(address);
    handler.write(handler.ctx, address, data);
}

uint8 MainBus::read(const uint16 address)
{
    auto& handler = getHandler(address);
    return handler.read(handler.ctx, address);
}
