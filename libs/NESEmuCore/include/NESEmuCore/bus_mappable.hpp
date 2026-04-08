#ifndef NESEMU_BUS_MAPPABLE_HPP
#define NESEMU_BUS_MAPPABLE_HPP

#include "emu_types.hpp"

namespace NESEmu {
template <typename TMapper>
class BusMappable {
public:
    [[nodiscard]] bool  canRead(const uint16 address) const { return static_cast<const TMapper&>(*this).isReadEnabled(address); }
    [[nodiscard]] bool  canWrite(const uint16 address) const { return static_cast<const TMapper&>(*this).isWriteEnabled(address); }
    [[nodiscard]] uint8 read(const uint16 address) const { return static_cast<const TMapper&>(*this).readBus(address); }
    void                write(const uint16 address, const uint8 data) { static_cast<TMapper&>(*this).writeBus(address, data); }
};
}

#endif //NESEMU_BUS_MAPPABLE_HPP
