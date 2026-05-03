#ifndef NESEMU_BUS_MAPPABLE_HPP
#define NESEMU_BUS_MAPPABLE_HPP

#include "emu_types.hpp"

namespace NESEmu {
template <typename TMapper>
class BusMappable {
public:
    [[nodiscard]] uint8 read(const uint16 address) { return static_cast<TMapper&>(*this).readBus(address); }
    void                write(const uint16 address, const uint8 data) { static_cast<TMapper&>(*this).writeBus(address, data); }
};
}

#endif //NESEMU_BUS_MAPPABLE_HPP
