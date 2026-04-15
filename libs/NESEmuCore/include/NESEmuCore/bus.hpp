#ifndef NESEMU_BUS_HPP
#define NESEMU_BUS_HPP

#include "NESEmuCore/emu_types.hpp"

/*
# References
https://www.nesdev.org/wiki/CE_Decoder
https://www.nesdev.org/wiki/CPU_memory_map

CPU Memory Map
---------------------------------------------------------------
|Address range	Size	Device
|$0000–$07FF	$0800	2 KB internal RAM
|• $0000-$00FF  $0100   The zero page, which can be accessed with fewer bytes and cycles than other addresses
|• $0100–$01FF  $0100   The page containing the stack range (typically starts at $01FF and grows downward)
|$0800–$0FFF	$0800	Mirrors of $0000–$07FF
|$1000–$17FF	$0800   Mirrors of $0000–$07FF
|$1800–$1FFF	$0800   Mirrors of $0000–$07FF
|$2000–$2007	$0008	NES PPU registers
|$2008–$3FFF	$1FF8	Mirrors of $2000–$2007 (repeats every 8 bytes)
|$4000–$4017	$0018	NES APU and I/O registers
|$4018–$401F	$0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
|$4020–$FFFF    $BFE0   Unmapped. Available for cartridge use.
|• $6000–$7FFF  $2000   Usually cartridge RAM, when present.
|• $8000–$FFFF  $8000   Usually cartridge ROM and mapper registers.
|• $FFFA        $0002   NMI vector
|• $FFFC        $0002   Reset vector
|• $FFFE        $0002   IRQ vector
*/

namespace NESEmu {
class InternalRam;

enum BusType {
    kBusDefault,
    kBusTest,
    kMaxBusTypes
};

class Bus {
public:
    [[nodiscard]] uint8 read(const uint16_t address) { return m_readFn[type](*this, address); }
    void                write(const uint16_t address, const uint8 data) { m_writeFn[type](*this, address, data); }

protected:
    explicit Bus(const BusType t) : type(t) {}

    using readFn                                  = uint8 (*)(Bus&, uint16_t);
    using writeFn                                 = void (*)(Bus&, uint16, uint8);
    inline static readFn  m_readFn[kMaxBusTypes]  = {};
    inline static writeFn m_writeFn[kMaxBusTypes] = {};

    const BusType type;
};

template <class T, BusType TBus>
class BusCRTP : public Bus {
    static const BusType type;

    static uint8 readBus(Bus& b, uint16 address)
    {
        return static_cast<T&>(b).read(address);
    }

    static void writeBus(Bus& b, uint16 address, uint8 data)
    {
        static_cast<T&>(b).write(address, data);
    }

    static constexpr BusType Register()
    {
        return m_readFn[TBus] = readBus, m_writeFn[TBus] = writeBus, TBus;
    }

protected:
    BusCRTP() : Bus(type) {}
};

template <class T, BusType TBus>
const BusType BusCRTP<T, TBus>::type = Register();

class DefaultBus : public BusCRTP<DefaultBus, kBusDefault> {
    static constexpr uint16 ADDRESS_MASK       = 0b1111100000000000;
    static constexpr uint16 MEMORY_ENABLE_MASK = 0b0000000000000000; // $0000–$1FFF
    static constexpr uint16 PPU_ENABLE_MASK    = 0b0010000000000000; // $2000–$3FFF
    static constexpr uint16 APU_IO_ENABLE_MASK = 0b0100000000000000; // $4000–$5FFF

public:
    explicit DefaultBus(InternalRam& memory) : m_memory(memory) {}

    [[nodiscard]] uint8 read(uint16 address);
    void                write(uint16 address, uint8 data);

private:
    InternalRam& m_memory;

    uint8 m_openBusData = 0;
};
}

#endif //NESEMU_BUS_HPP
