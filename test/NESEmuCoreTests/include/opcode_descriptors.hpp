#ifndef NESEMU_OPCODE_DESCRIPTORS_HPP
#define NESEMU_OPCODE_DESCRIPTORS_HPP

#include "NESEmuCore/emu_types.hpp"

#include <string>
#include <vector>

using namespace NESEmu;

// Addressing mode descriptors:
// (https://www.nesdev.org/obelisk-6502-guide/addressing.html)
//-----------------------------
// Implied
// Accumulator
// Immediate
// Zero Page
// Zero Page,X
// Zero Page,Y
// Relative
// Absolute
// Absolute,X
// Absolute,Y
// Indirect,X
// Indirect,Y

inline std::vector<std::tuple<uint8, std::string>> opcodeDescriptionList = {
    { 0x61, "$61 ADC (indirect,X)" },
    { 0x69, "$69 ADC (immediate)" },
    { 0x6d, "$6d ADC (absolute)" },
    { 0x71, "$71 ADC (indirect,Y)" },
    { 0x75, "$75 ADC (zero page,x)" },
    { 0x79, "$79 ADC (absolute,Y)" },
    { 0x7d, "$7d ADC (absolute,x)" },
    { 0xc8, "$c8 INY (implied)" },
    { 0xe8, "$e8 INX (implied)" },
    { 0xea, "$ea NOP" },
};

#endif //NESEMU_OPCODE_DESCRIPTORS_HPP
