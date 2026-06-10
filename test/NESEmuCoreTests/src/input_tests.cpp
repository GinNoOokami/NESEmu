#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/clock.hpp"
#include "NESEmuCore/controller.hpp"
#include "NESEmuCore/interrupt_lines.hpp"
#include "NESEmuCore/rp2a03.hpp"

#include <doctest.h>

using namespace NESEmu;

constexpr uint16 kMMIOJoypadCtrl = 0x4016;
constexpr uint16 kMMIOJoypad1    = 0x4016;
constexpr uint16 kMMIOJoypad2    = 0x4017;

namespace {
// Drive the standard strobe sequence: 1 latches-while-high, 0 freezes the snapshot.
void strobe(Rp2A03& cpu)
{
    cpu.onCpuWrite(kMMIOJoypadCtrl, 0x01);
    cpu.onCpuWrite(kMMIOJoypadCtrl, 0x00);
}

// Assemble the first 8 serial reads LSB-first: read 0 -> bit 0, read 7 -> bit 7.
// With this ordering the readback byte equals the latched button mask directly.
uint8 readByte(Rp2A03& cpu, uint16 reg)
{
    uint8 out = 0;
    for (int i = 0; i < 8; ++i) {
        out |= static_cast<uint8>((cpu.onCpuRead(reg) & 0x01) << i);
    }
    return out;
}

void press(Controller& controller, uint8 mask)
{
    for (int i = 0; i < 8; ++i) {
        if (mask & (1 << i)) {
            controller.simulatePress(static_cast<InputButtons>(1 << i));
        }
    }
}
}

TEST_SUITE("Input Tests") {
TEST_CASE("Controller serial read")
{
    Clock          clock;
    MainBus        mainBus;
    InterruptLines interruptLines;
    Rp2A03         cpu(clock, mainBus, interruptLines);
    Controller     controller1;
    Controller     controller2;

    cpu.startup();
    cpu.attachController(Joypad1, &controller1);
    cpu.attachController(Joypad2, &controller2);

    // Each port behaves identically; only the register and target controller differ.
    struct PortCase {
        uint16      reg;
        InputPort   port;
        const char* name;
    };
    constexpr PortCase portCases[] = {
        { kMMIOJoypad1, Joypad1, "controller 1 ($4016)" },
        { kMMIOJoypad2, Joypad2, "controller 2 ($4017)" },
    };
    Controller* controllers[] = { &controller1, &controller2 };

    for (const auto& portCase : portCases) {
        Controller& controller = *controllers[portCase.port];

        SUBCASE(portCase.name) {
            // Position: one button -> exactly one bit at its index, all others clear.
            SUBCASE("each button maps to its serial bit position") {
                struct ButtonCase {
                    InputButtons button;
                    uint8        expected; // independent literal, not derived from the enum
                    const char*  name;
                };
                constexpr ButtonCase cases[] = {
                    { InputButtons::A,      0x01, "A      -> read 0" },
                    { InputButtons::B,      0x02, "B      -> read 1" },
                    { InputButtons::Select, 0x04, "Select -> read 2" },
                    { InputButtons::Start,  0x08, "Start  -> read 3" },
                    { InputButtons::Up,     0x10, "Up     -> read 4" },
                    { InputButtons::Down,   0x20, "Down   -> read 5" },
                    { InputButtons::Left,   0x40, "Left   -> read 6" },
                    { InputButtons::Right,  0x80, "Right  -> read 7" },
                };
                for (const auto& c : cases) {
                    SUBCASE(c.name) {
                        controller.simulatePress(c.button);
                        strobe(cpu);

                        CHECK_EQ(readByte(cpu, portCase.reg), c.expected);
                    }
                }
            }

            // Interaction: combinations must coexist with no cross-bleed during the shift.
            SUBCASE("button combinations read back without cross-talk") {
                struct ComboCase {
                    uint8       mask;
                    const char* name;
                };
                constexpr ComboCase cases[] = {
                    { 0x00, "none pressed" },
                    { 0x03, "A+B" },
                    { 0x81, "A+Right (jump while moving)" },
                    { 0x0C, "Start+Select" },
                    { 0x0E, "B+Select+Start (adjacent bits)" },
                    { 0x30, "Up+Down (opposing, passes through)" },
                    { 0xC0, "Left+Right (opposing, passes through)" },
                    { 0xF0, "full D-pad" },
                    { 0xFF, "all pressed" },
                };
                for (const auto& c : cases) {
                    SUBCASE(c.name) {
                        press(controller, c.mask);
                        strobe(cpu);

                        CHECK_EQ(readByte(cpu, portCase.reg), c.mask);
                    }
                }
            }

            SUBCASE("ninth and later reads return 1") {
                press(controller, 0x00); // body all zero so trailing 1s are unambiguous
                strobe(cpu);

                (void)readByte(cpu, portCase.reg); // consume the 8 real bits

                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 1);
                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 1);
                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 1);
            }

            // The latched snapshot is frozen at strobe-low; mid-readout presses must not leak in.
            SUBCASE("input changes after latch do not alter the in-flight shift-out") {
                controller.simulatePress(InputButtons::A);
                strobe(cpu);

                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 1); // bit 0: A

                controller.simulatePress(InputButtons::B); // pressed mid-readout

                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 0); // bit 1 still the A-only snapshot
            }

            SUBCASE("unplugged port reads 0") {
                cpu.attachController(portCase.port, nullptr);
                strobe(cpu);

                CHECK_EQ(cpu.onCpuRead(portCase.reg) & 0x01, 0);
            }
        }
    }
}

TEST_CASE("Controller strobe")
{
    Clock          clock;
    MainBus        mainBus;
    InterruptLines interruptLines;
    Rp2A03         cpu(clock, mainBus, interruptLines);
    Controller     controller1;
    Controller     controller2;

    cpu.startup();
    cpu.attachController(Joypad1, &controller1);
    cpu.attachController(Joypad2, &controller2);

    // While the strobe is held high the latch is transparent: each read re-samples live input.
    SUBCASE("strobe held high continuously reloads the latch") {
        cpu.onCpuWrite(kMMIOJoypadCtrl, 0x01);

        controller1.simulatePress(InputButtons::A);
        CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 1);
        CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 1); // reloaded, not shifted away

        controller1.simulateRelease(InputButtons::A);
        CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 0); // release seen immediately
    }

    // A $4016 write strobes BOTH pads; verify controller 2 latches from it (read via $4017).
    SUBCASE("$4016 write latches both controllers") {
        controller2.simulatePress(InputButtons::Start);
        strobe(cpu); // writes target $4016 only

        CHECK_EQ(readByte(cpu, kMMIOJoypad2), 0x08);
    }

    // RED: $4017 writes go to the APU frame counter and must NOT strobe the pads.
    // Currently writeController fires for both addresses, so this fails until routing is fixed.
    SUBCASE("$4017 write does not strobe the controllers") {
        controller1.simulatePress(InputButtons::A);
        strobe(cpu); // legitimate $4016 strobe -> latched = A

        controller1.simulatePress(InputButtons::B); // changes live state only

        cpu.onCpuWrite(kMMIOJoypad2, 0x01); // must NOT re-latch
        cpu.onCpuWrite(kMMIOJoypad2, 0x00);

        CHECK_EQ(readByte(cpu, kMMIOJoypad1), 0x01); // still the A-only snapshot, not A+B
    }
}

TEST_CASE("Controller reset")
{
    Controller controller;

    SUBCASE("clears strobing and simulated input") {
        controller.startPoll();
        controller.simulatePress(InputButtons::A);

        controller.reset();

        CHECK_FALSE(controller.strobing());
        CHECK_EQ(controller.simulatedState(), 0);
    }

    // RED: reset should also clear the shift register; currently m_latchedState survives reset.
    SUBCASE("clears the shift register") {
        controller.simulatePress(InputButtons::A);
        controller.startPoll();
        controller.stopPoll(); // latched = A

        controller.reset();

        CHECK_EQ(controller.latchedState(), 0);
    }
}
}
