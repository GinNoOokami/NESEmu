#ifndef NESEMU_TEST_BUS_HPP
#define NESEMU_TEST_BUS_HPP

#include <array>

namespace NESEmu {
class TestRam {
public:
    [[nodiscard]] uint8 read(uint16_t address) const { return m_memory[address]; }
    void                write(uint16 address, uint8 data) { m_memory[address] = data; }

private:
    // Tests can use the full 64kb address space
    std::array<uint8, 0x10000> m_memory{};
};
}

#endif //NESEMU_TEST_BUS_HPP
