#ifndef NESEMU_BUS_HPP
#define NESEMU_BUS_HPP

#include "NESEmuCore/cartridge.hpp"
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
template <typename T>
concept BusDevice = requires(T& device, uint16 addr, uint8 data)
{
    { device.read(addr) } -> std::same_as<uint8>;
    { device.write(addr, data) };
};

enum class AddressRegion : uint8 {
    WorkRam,
    Ppu,
    ApuIo,
    Cartridge0,
    Cartridge1,
    Cartridge2,
    Cartridge3,
    Cartridge4,
};

class MainBus {
public:
    template <BusDevice T>
    void attachRegion(AddressRegion address, T& dev) { m_regions[static_cast<uint8>(address)] = makeHandler(dev); }

    void                write(uint16 address, uint8 data);
    [[nodiscard]] uint8 read(uint16 address);

private:
    static constexpr int kAddressRegionCount = 8;
    static constexpr int kAddressRegionMask  = kAddressRegionCount - 1;

    struct Handler {
        uint8 (*read)(void*, uint16)         = &Handler::readUnmapped;
        void (* write)(void*, uint16, uint8) = &Handler::writeUnmapped;
        void*   ctx                          = nullptr;

        static uint8 readUnmapped(void*, uint16) { return 0; }
        static void  writeUnmapped(void*, uint16, uint8) {}
    };

    template <BusDevice T>
    static constexpr Handler makeHandler(T& device)
    {
        return {
            +[](void* ctx, uint16 addr) { return static_cast<T*>(ctx)->read(addr); },
            +[](void* ctx, uint16 addr, uint8 data) { static_cast<T*>(ctx)->write(addr, data); },
            &device
        };
    }

    [[nodiscard]] const Handler& getHandler(uint16 address) const
    {
        // The CE select uses the 3 highest bits on the address line
        return m_regions[(address >> 13) & kAddressRegionMask];
    }

    Handler m_regions[kAddressRegionCount];
};

class PpuBus {
    void write(uint16 address, uint8 data) const
    {
        /* Placeholder */
    }

    [[nodiscard]] uint8 read(uint16 address) { return 0; }
};
}

#endif //NESEMU_BUS_HPP
