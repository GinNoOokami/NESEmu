#ifndef NESEMU_BUS_HPP
#define NESEMU_BUS_HPP

#include "NESEmuCore/emu_types.hpp"

/*
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
#ifndef RUN_TESTS
    class InternalRam;
    using Memory = InternalRam;
#else
    class FlatMemory;
    using Memory = FlatMemory;
#endif

    class Bus {
    public:
        explicit Bus(Memory* memory) : m_memory(memory) {}
        ~Bus() = default;

        uint8   read(uint16 address);
        void    write(uint16 address, uint8 data);

    private:
        Memory* m_memory;

        uint8 m_openBusData = 0;
    };
}

#endif //NESEMU_BUS_HPP
