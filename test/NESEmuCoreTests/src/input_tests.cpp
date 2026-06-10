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

TEST_SUITE("Input Tests") {
TEST_CASE("Controller input")
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

    SUBCASE("when resetting") {
        SUBCASE("resets simulated button presses") {
            controller1.simulatePress(InputButtons::A);
            controller1.reset();

            CHECK_EQ(controller1.simulatedState(), 0);
        }
        SUBCASE("stops strobing") {
            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x01);
            controller1.reset();

            CHECK_FALSE(controller1.strobing());
        }
    }

    SUBCASE("when strobing") {
        cpu.onCpuWrite(kMMIOJoypadCtrl, 0x01);

        SUBCASE("write 1 to $4016 continuously reloads shift register") {
            controller1.simulatePress(InputButtons::A);

            CHECK(controller1.strobing());
            CHECK(controller1.readControllerInputBit());

            // Check a second time to ensure the shift register has reloaded during strobing
            CHECK(controller1.readControllerInputBit());

            controller1.simulateRelease(InputButtons::A);

            // Ensure releasing the key is also picked up during strobing
            CHECK_FALSE(controller1.readControllerInputBit());
        }
        SUBCASE("write 0 to $4016 stops reloading and latches input state") {
            controller1.simulatePress(InputButtons::A);
            controller1.simulatePress(InputButtons::B);

            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x0);

            CHECK_FALSE(controller1.strobing());
            CHECK_EQ(controller1.latchedState(), 3);
        }
        SUBCASE("first 8 reads of $4016 returns first bit of latched state from controller 1") {
            controller1.simulatePress(InputButtons::A);
            controller1.simulatePress(InputButtons::B);
            controller1.simulatePress(InputButtons::Up);
            controller1.simulatePress(InputButtons::Left);
            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x00);

            uint8 inputState = 0;
            for (auto i = 0; i < 8; i++) {
                bool bit   = cpu.onCpuRead(kMMIOJoypad1) & 0x01;
                inputState <<= 1;
                inputState |= bit;
            }

            // A, B, ---, ---, UP, ---, LEFT, ---
            CHECK_EQ(inputState, 0b11001010);
        }
        SUBCASE("subsequent reads of $4016 returns 1 from controller 1") {
            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x00);

            for (auto i = 0; i < 8; i++) {
                (void)cpu.onCpuRead(kMMIOJoypad1);
            }

            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 1);
            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 1);
            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad1) & 0x01, 1);
        }
        SUBCASE("first 8 reads of $4017 returns first bit of latched state from controller 2") {
            controller2.simulatePress(InputButtons::A);
            controller2.simulatePress(InputButtons::B);
            controller2.simulatePress(InputButtons::Up);
            controller2.simulatePress(InputButtons::Left);
            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x00);

            uint8 inputState = 0;
            for (auto i = 0; i < 8; i++) {
                bool bit   = cpu.onCpuRead(kMMIOJoypad2) & 0x01;
                inputState <<= 1;
                inputState |= bit;
            }

            // A, B, ---, ---, UP, ---, LEFT, ---
            CHECK_EQ(inputState, 0b11001010);
        }
        SUBCASE("subsequent reads of $4017 returns 1 from controller 2") {
            cpu.onCpuWrite(kMMIOJoypadCtrl, 0x00);

            for (auto i = 0; i < 8; i++) {
                (void)cpu.onCpuRead(kMMIOJoypad2);
            }

            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad2) & 0x01, 1);
            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad2) & 0x01, 1);
            CHECK_EQ(cpu.onCpuRead(kMMIOJoypad2) & 0x01, 1);
        }
    }
}
}
