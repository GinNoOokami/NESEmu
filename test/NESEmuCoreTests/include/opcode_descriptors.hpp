#ifndef NESEMU_OPCODE_DESCRIPTORS_HPP
#define NESEMU_OPCODE_DESCRIPTORS_HPP

#include "NESEmuCore/emu_types.hpp"

#include <vector>

using namespace NESEmu;

// Addressing mode descriptors:
// (https://www.nesdev.org/obelisk-6502-guide/addressing.html)
//-----------------------------
// Implicit
// Accumulator
// Immediate
// Zero Page
// Zero Page,X
// Zero Page,Y
// Relative
// Absolute
// Absolute,X
// Absolute,Y
// Indirect
// Indexed Indirect
// Indirect Indexed

inline std::vector<std::tuple<uint8, std::string>> opcodeDescriptionList = {
    {0xc8, "$c8 INY (Implicit)"},
    {0xe8, "$e8 INX (Implicit)"},
    {0xea, "$ea NOP (Implicit)"},
};

#endif //NESEMU_OPCODE_DESCRIPTORS_HPP
