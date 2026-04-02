#include "doctest.h"

#include "NESEmuCore/cpu_6502.hpp"

TEST_CASE("CPU Init State") {
    CPU_6502 cpu;
    cpu.startup();

    CHECK_EQ(cpu.state().sp, 0xFD);
    CHECK(cpu.state().p);
    CHECK_EQ(cpu.state().a, 0);
    CHECK_EQ(cpu.state().x, 0);
    CHECK_EQ(cpu.state().y, 0);
}
