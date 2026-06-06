#include "NESEmuCore/ppu.hpp"
#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/interrupt_lines.hpp"

#include <doctest.h>

using namespace NESEmu;

constexpr uint16 kPpuCtrl   = 0x2000;
constexpr uint16 kPpuMask   = 0x2001;
constexpr uint16 kPpuScroll = 0x2005;
constexpr uint16 kPpuAddr   = 0x2006;

TEST_SUITE("PPU Tests") {
TEST_CASE("PPUCTRL")
{
    constexpr uint16 ppuCtrl = 0x2000;
    CiRam            ciram;
    PpuBus           ppuBus(ciram);
    InterruptLines   interruptLines{};
    Ppu              ppu(ppuBus, interruptLines);

    SUBCASE("write") {
        SUBCASE("baseNametableAddress returns expected") {
            SUBCASE("$2000") {
                ppu.onCpuWrite(ppuCtrl, 0xF0);

                CHECK_EQ(ppu.ppuCtrl().baseNametableAddress(), 0);
            }
            SUBCASE("$2400") {
                ppu.onCpuWrite(ppuCtrl, 0xF1);

                CHECK_EQ(ppu.ppuCtrl().baseNametableAddress(), 1);
            }

            SUBCASE("$2800") {
                ppu.onCpuWrite(ppuCtrl, 0xF2);

                CHECK_EQ(ppu.ppuCtrl().baseNametableAddress(), 2);
            }
            SUBCASE("$2C00") {
                ppu.onCpuWrite(ppuCtrl, 0xF3);

                CHECK_EQ(ppu.ppuCtrl().baseNametableAddress(), 3);
            }
        }

        SUBCASE("ramAddressIncrement returns expected") {
            SUBCASE("across") {
                ppu.onCpuWrite(ppuCtrl, 0xFB);

                CHECK_FALSE(ppu.ppuCtrl().addressIncrementMode());
            }
            SUBCASE("down") {
                ppu.onCpuWrite(ppuCtrl, 0xF4);

                CHECK(ppu.ppuCtrl().addressIncrementMode());
            }
        }

        SUBCASE("spritePatternTableAddress returns expected") {
            SUBCASE("$0000") {
                ppu.onCpuWrite(ppuCtrl, 0xF7);

                CHECK_FALSE(ppu.ppuCtrl().spritePatternTableAddress());
            }
            SUBCASE("$1000") {
                ppu.onCpuWrite(ppuCtrl, 0xF8);

                CHECK(ppu.ppuCtrl().spritePatternTableAddress());
            }
        }

        SUBCASE("backgroundPatternTableAddress returns expected") {
            SUBCASE("$0000") {
                ppu.onCpuWrite(ppuCtrl, 0xEF);

                CHECK_FALSE(ppu.ppuCtrl().backgroundPatternTableAddress());
            }
            SUBCASE("$1000") {
                ppu.onCpuWrite(ppuCtrl, 0xFF);

                CHECK(ppu.ppuCtrl().backgroundPatternTableAddress());
            }
        }

        SUBCASE("spriteSize returns expected") {
            SUBCASE("8x8") {
                ppu.onCpuWrite(ppuCtrl, 0xDF);

                CHECK_FALSE(ppu.ppuCtrl().spriteSize());
            }
            SUBCASE("8x16") {
                ppu.onCpuWrite(ppuCtrl, 0x7F);

                CHECK(ppu.ppuCtrl().spriteSize());
            }
        }

        SUBCASE("masterSlaveSelect returns expected") {
            SUBCASE("read backdrop") {
                ppu.onCpuWrite(ppuCtrl, 0xAF);

                CHECK_FALSE(ppu.ppuCtrl().masterSlaveSelect());
            }
            SUBCASE("output color") {
                ppu.onCpuWrite(ppuCtrl, 0xFF);
                CHECK(ppu.ppuCtrl().masterSlaveSelect());
            }
        }

        SUBCASE("enableNmi returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuCtrl, 0x7F);

                CHECK_FALSE(ppu.ppuCtrl().nmiEnable());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuCtrl, 0xFF);

                CHECK(ppu.ppuCtrl().nmiEnable());
            }
        }
        SUBCASE("enableNmi triggers NMI during vBlank") {
            ppu.executeUntil(Ppu::kFrameScanlineWidth * 241 + 1);

            CHECK(ppu.ppuStatus().vBlank());
            CHECK_FALSE(interruptLines.nmiActive);

            ppu.onCpuWrite(ppuCtrl, 0x80);

            CHECK(interruptLines.nmiActive);
        }
    }
    SUBCASE("reads open bus latch") {
        ppu.onCpuWrite(ppuCtrl, 0x55);

        CHECK_EQ(ppu.onCpuRead(ppuCtrl), 0x55);
    }
}

TEST_CASE("PPUMASK")
{
    constexpr uint16 ppuMask = 0x2001;
    CiRam            ciram;
    PpuBus           ppuBus(ciram);
    InterruptLines   interruptLines{};
    Ppu              ppu(ppuBus, interruptLines);

    SUBCASE("write") {
        SUBCASE("greyscale returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0xF0);

                CHECK_FALSE(ppu.ppuMask().greyscale());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0xF1);

                CHECK(ppu.ppuMask().greyscale());
            }
        }

        SUBCASE("backgroundColumnMask returns expected") {
            SUBCASE("hidden") {
                ppu.onCpuWrite(ppuMask, 0xF1);

                CHECK_FALSE(ppu.ppuMask().backgroundColumnMask());
            }
            SUBCASE("shown") {
                ppu.onCpuWrite(ppuMask, 0xF2);

                CHECK(ppu.ppuMask().backgroundColumnMask());
            }
        }
        SUBCASE("spriteColumnMask returns expected") {
            SUBCASE("hidden") {
                ppu.onCpuWrite(ppuMask, 0xF1);

                CHECK_FALSE(ppu.ppuMask().spriteColumnMask());
            }
            SUBCASE("shown") {
                ppu.onCpuWrite(ppuMask, 0xF4);

                CHECK(ppu.ppuMask().spriteColumnMask());
            }
        }

        SUBCASE("backgroundEnabled returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0xF3);

                CHECK_FALSE(ppu.ppuMask().backgroundEnabled());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0xF8);

                CHECK(ppu.ppuMask().backgroundEnabled());
            }
        }

        SUBCASE("spriteEnabled returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0x0F);

                CHECK_FALSE(ppu.ppuMask().spriteEnabled());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0x1F);

                CHECK(ppu.ppuMask().spriteEnabled());
            }
        }

        SUBCASE("emphasizeRed returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0x1F);

                CHECK_FALSE(ppu.ppuMask().emphasizeRed());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0x2F);

                CHECK(ppu.ppuMask().emphasizeRed());
            }
        }

        SUBCASE("emphasizeGreen returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0x3F);

                CHECK_FALSE(ppu.ppuMask().emphasizeGreen());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0x4F);

                CHECK(ppu.ppuMask().emphasizeGreen());
            }
        }

        SUBCASE("emphasizeBlue returns expected") {
            SUBCASE("disabled") {
                ppu.onCpuWrite(ppuMask, 0x7F);

                CHECK_FALSE(ppu.ppuMask().emphasizeBlue());
            }
            SUBCASE("enabled") {
                ppu.onCpuWrite(ppuMask, 0x8F);

                CHECK(ppu.ppuMask().emphasizeBlue());
            }
        }
    }

    SUBCASE("reads open bus latch") {
        ppu.onCpuWrite(ppuMask, 0x55);

        CHECK_EQ(ppu.onCpuRead(ppuMask), 0x55);
    }
}

TEST_CASE("PPUSTATUS")
{
    constexpr uint16 ppuStatus = 0x2002;
    CiRam            ciram;
    PpuBus           ppuBus(ciram);
    InterruptLines   interruptLines{};
    Ppu              ppu(ppuBus, interruptLines);

    SUBCASE("write does not affect register bits") {
        ppu.onCpuWrite(ppuStatus, 0xFF);

        CHECK_EQ(ppu.onCpuRead(ppuStatus) & 0x1110'0000, 0);
    }

    SUBCASE("write returns open data latch on non-register bits") {
        ppu.onCpuWrite(ppuStatus, 0xFF);

        // Lower 5 bits should return open bus data latch, which is set during the write
        CHECK_EQ(ppu.onCpuRead(ppuStatus), 0x1F);
    }

    SUBCASE("read vBlank") {
        SUBCASE("on first visible frame (scanline 0) is unset") {
            ppu.reset();

            ppu.executeUntil(1);

            CHECK_FALSE(ppu.ppuStatus().vBlank());
        }

        SUBCASE("on first post-render frame (scanline 240) is unset") {
            ppu.reset();

            ppu.executeUntil(Ppu::kFrameScanlineWidth * 240);

            CHECK_FALSE(ppu.ppuStatus().vBlank());
        }

        SUBCASE("on first vBlank frame (scanline 241) is set") {
            ppu.reset();

            // Although we aren't accounting for it right now, technically
            // the vBlank flag is set on the first non-idle cycle of the scanline
            ppu.executeUntil(Ppu::kFrameScanlineWidth * 241 + 1);

            CHECK(ppu.ppuStatus().vBlank());
        }

        SUBCASE("on pre-render frame (scanline 261) is unset") {
            ppu.reset();

            // Although we aren't accounting for it right now, technically
            // the vBlank flag is set on the first non-idle cycle of the scanline
            ppu.executeUntil(Ppu::kFrameScanlineWidth * 261 + 1);

            CHECK_FALSE(ppu.ppuStatus().vBlank());
        }

        SUBCASE("clears vBlank bit") {
            ppu.reset();

            // Put the PPU in vBlank state and read the status register
            ppu.executeUntil(Ppu::kFrameScanlineWidth * 242);
            auto _ = ppu.onCpuRead(ppuStatus);

            CHECK_FALSE(ppu.ppuStatus().vBlank());
        }
    }
}

TEST_CASE("OAMADDR/OAMDATA")
{
    constexpr uint16 oamAddr = 0x2003;
    constexpr uint16 oamData = 0x2004;
    CiRam            ciram;
    PpuBus           ppuBus(ciram);
    InterruptLines   interruptLines{};
    Ppu              ppu(ppuBus, interruptLines);

    SUBCASE("OAMADDR write sets internal address") {
        ppu.onCpuWrite(oamAddr, 0x10);

        CHECK((ppu.onCpuRead(oamAddr) == 0x10));
    }

    SUBCASE("OAMDATA write increments OAMADDR") {
        ppu.onCpuWrite(oamAddr, 0x00);
        ppu.onCpuWrite(oamData, 0xAB);

        CHECK((ppu.onCpuRead(oamAddr) == 0x01));
    }

    SUBCASE("OAMDATA write wraps address from 0xFF to 0x00") {
        ppu.onCpuWrite(oamAddr, 0xFF);
        ppu.onCpuWrite(oamData, 0xCD);

        CHECK((ppu.onCpuRead(oamAddr) == 0x00));
    }

    SUBCASE("OAMDATA read returns byte at current address") {
        ppu.onCpuWrite(oamAddr, 0x20);
        ppu.onCpuWrite(oamData, 0x5A);
        ppu.onCpuWrite(oamAddr, 0x20);

        CHECK((ppu.onCpuRead(oamData) == 0x5A));
    }
}

TEST_CASE("PPUSCROLL")
{
    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines{};
    Ppu            ppu(ppuBus, interruptLines);
    ppu.startup();

    SUBCASE("first write sets w to 1") {
        auto prevState = ppu.internalRegisters().w;
        CHECK_EQ(ppu.internalRegisters().w, 0);

        ppu.onCpuWrite(kPpuScroll, 0);

        CHECK_EQ(ppu.internalRegisters().w, 1);
    }
    SUBCASE("first write copies lower bits to x and upper bits to coarseX bits of t") {
        ppu.onCpuWrite(kPpuScroll, 0xA5);

        CHECK_EQ(ppu.internalRegisters().t.coarseX(), 20);
        CHECK_EQ(ppu.internalRegisters().x, 5);
    }
    SUBCASE("second write sets w back to 0") {
        auto prevState = ppu.internalRegisters().w;
        CHECK_EQ(ppu.internalRegisters().w, 0);

        ppu.onCpuWrite(kPpuScroll, 0);
        ppu.onCpuWrite(kPpuScroll, 0);

        CHECK_EQ(ppu.internalRegisters().w, 0);
    }
    SUBCASE("second copies lower bits to coarseY and upper bits to fineY of t") {
        ppu.onCpuWrite(kPpuScroll, 0);
        ppu.onCpuWrite(kPpuScroll, 0xA5);

        CHECK_EQ(ppu.internalRegisters().t.coarseY(), 20);
        CHECK_EQ(ppu.internalRegisters().t.fineY(), 5);
    }
}

TEST_CASE("PPUADDR/PPUDATA")
{
    constexpr uint16 ppuCtrl   = 0x2000;
    constexpr uint16 ppuStatus = 0x2002;
    constexpr uint16 ppuAddr   = 0x2006;
    constexpr uint16 ppuData   = 0x2007;

    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines{};
    Ppu            ppu(ppuBus, interruptLines);
    ppu.startup();

    SUBCASE("power-on state: v, t, x, w are zero") {
        auto [v, t, x, w] = ppu.internalRegisters();

        CHECK_EQ(v, 0);
        CHECK_EQ(t, 0);
        CHECK_EQ(x, 0);
        CHECK_EQ(w, 0);
    }

    SUBCASE("PPUSTATUS read clears w and leaves t, v, x unchanged") {
        // Ensure w bit is set at start
        ppu.onCpuWrite(ppuAddr, 0x00);
        CHECK(ppu.internalRegisters().w);
        auto prevState = ppu.internalRegisters();

        (void)ppu.onCpuRead(ppuStatus);

        auto [v, t, x, w] = ppu.internalRegisters();
        CHECK_FALSE(w);
        CHECK_EQ(t, prevState.t);
        CHECK_EQ(v, prevState.v);
        CHECK_EQ(x, prevState.x);
    }

    SUBCASE("PPUCTRL write copies nametable select into t bits 10-11 without affecting v, w, or x") {
        auto prevState = ppu.internalRegisters();

        ppu.onCpuWrite(ppuCtrl, 0x03);

        auto [v, t, x, w] = ppu.internalRegisters();
        CHECK_EQ(w, prevState.w);
        CHECK_EQ(v, prevState.v);
        CHECK_EQ(x, prevState.x);
        CHECK_EQ(t.lsb(), prevState.t.lsb());
        CHECK_EQ(t.msb() & 0x0C, 0x0C);
        CHECK_EQ(t.msb() & 0xF3, prevState.t.msb() & 0xF3);
    }

    SUBCASE("PPUADDR first write sets t high byte, clears t bit 14, discards upper 2 bits, sets w, leaves v and x unchanged") {
        auto prevState = ppu.internalRegisters();
        ppu.onCpuWrite(ppuAddr, 0xFF);

        auto [v, t, x, w] = ppu.internalRegisters();
        CHECK(w);
        CHECK_EQ(v, prevState.v);
        CHECK_EQ(x, prevState.x);
        CHECK_EQ(t.msb(), 0x3F);
        CHECK_EQ(t.lsb(), prevState.t.lsb());
    }

    SUBCASE("PPUADDR second write sets t low byte, copies t into v, clears w") {
        auto prevState = ppu.internalRegisters();
        ppu.onCpuWrite(ppuAddr, 0xFF);
        ppu.onCpuWrite(ppuAddr, 0x12);

        auto [v, t, x, w] = ppu.internalRegisters();
        CHECK_FALSE(w);
        CHECK_EQ(v, t);
        CHECK_EQ(x, prevState.x);
        CHECK_EQ(t.lsb(), 0x12);
    }

    SUBCASE("PPUADDR write sequence interrupted by PPUSTATUS read resets w mid-sequence") {
        auto prevState = ppu.internalRegisters();
        ppu.onCpuWrite(ppuAddr, 0xFF);
        (void)ppu.onCpuRead(ppuStatus);
        ppu.onCpuWrite(ppuAddr, 0x20);

        auto [v, t, x, w] = ppu.internalRegisters();
        CHECK(w);
        CHECK_EQ(t.msb(), 0x20);
        CHECK_EQ(t.lsb(), prevState.t.lsb());
        CHECK_EQ(v, prevState.v);
    }

    SUBCASE("PPUDATA write stores byte at v and increments v") {
        SUBCASE("by 1 when PPUCTRL increment mode is across") {
            ppu.onCpuWrite(ppuAddr, 0x20);
            ppu.onCpuWrite(ppuAddr, 0x00);
            ppu.onCpuWrite(ppuCtrl, 0x00);

            ppu.onCpuWrite(ppuData, 0x55);

            auto v = ppu.internalRegisters().v;
            CHECK_EQ(v, 0x2001);
            CHECK_EQ(ppuBus.read(0x2000), 0x55);
        }

        SUBCASE("by 32 when PPUCTRL increment mode is down") {
            ppu.onCpuWrite(ppuAddr, 0x20);
            ppu.onCpuWrite(ppuAddr, 0x00);
            ppu.onCpuWrite(ppuCtrl, 0x04);

            ppu.onCpuWrite(ppuData, 0x55);

            auto v = ppu.internalRegisters().v;
            CHECK_EQ(v, 0x2020);
            CHECK_EQ(ppuBus.read(0x2000), 0x55);
        }
    }

    SUBCASE("PPUDATA read returns buffered value, refills buffer from v & 0x3FFF, and increments v") {
        SUBCASE("by 1 when PPUCTRL increment mode is across") {
            ppu.onCpuWrite(ppuAddr, 0x20);
            ppu.onCpuWrite(ppuAddr, 0x00);
            ppu.onCpuWrite(ppuCtrl, 0x00);

            // First read primes the read buffer, second read returns the actual data
            ppuBus.write(0x2000, 0x55);
            (void)ppu.onCpuRead(ppuData);
            auto data = ppu.onCpuRead(ppuData);

            auto v = ppu.internalRegisters().v;
            CHECK_EQ(v.busAddress(), 0x2002);
            CHECK_EQ(data, 0x55);
        }
        SUBCASE("by 32 when PPUCTRL increment mode is down") {
            ppu.onCpuWrite(ppuAddr, 0x20);
            ppu.onCpuWrite(ppuAddr, 0x00);
            ppu.onCpuWrite(ppuCtrl, 0x04);

            ppuBus.write(0x2000, 0x55);

            // First read primes the read buffer, second read returns the actual data
            (void)ppu.onCpuRead(ppuData);
            auto data = ppu.onCpuRead(ppuData);

            auto v = ppu.internalRegisters().v;
            CHECK_EQ(v.busAddress(), 0x2040);
            CHECK_EQ(data, 0x55);
        }
    }

    SUBCASE("PPUDATA palette read returns data immediately and refills buffer from nametable mirror") {
        ppu.onCpuWrite(ppuAddr, 0x3F);
        ppu.onCpuWrite(ppuAddr, 0x00);
        ppu.onCpuWrite(ppuCtrl, 0x00);
        ppu.onCpuWrite(ppuData, 0x55);
        ppu.onCpuWrite(ppuAddr, 0x3F);
        ppu.onCpuWrite(ppuAddr, 0x00);
        ppuBus.write(0x2F00, 0xA5);

        auto data = ppu.onCpuRead(ppuData);

        auto v = ppu.internalRegisters().v;
        CHECK_EQ(v.busAddress(), 0x3F01);
        CHECK_EQ(data, 0x55);

        // Reading any valid non-palette address in the PPU memory range should return the value in the read buffer
        ppu.onCpuWrite(ppuAddr, 0x20);
        ppu.onCpuWrite(ppuAddr, 0x00);
        auto mirroredNametableValue = ppu.onCpuRead(ppuData);

        CHECK_EQ(mirroredNametableValue, 0xA5);
    }

    SUBCASE("PPUDATA palette read returns mirrored value above $3F1F") {
        ppu.onCpuWrite(ppuAddr, 0x3F);
        ppu.onCpuWrite(ppuAddr, 0x00);
        ppu.onCpuWrite(ppuCtrl, 0x00);
        ppu.onCpuWrite(ppuData, 0x55);
        ppu.onCpuWrite(ppuData, 0xA5);
        ppu.onCpuWrite(ppuAddr, 0x3F);
        ppu.onCpuWrite(ppuAddr, 0x20);

        (void)ppu.onCpuRead(ppuData);
        auto data = ppu.onCpuRead(ppuData);

        CHECK_EQ(data, 0xA5);
    }

    SUBCASE("PPUDATA increment stride changes when PPUCTRL is rewritten between accesses") {
        ppu.onCpuWrite(ppuAddr, 0x20);
        ppu.onCpuWrite(ppuAddr, 0x00);
        ppu.onCpuWrite(ppuCtrl, 0x00);

        ppu.onCpuWrite(ppuData, 0x55);

        // Check that the first write is committed and the address increments by 1
        CHECK_EQ(ppuBus.read(0x2000), 0x55);
        CHECK_EQ(ppu.internalRegisters().v.busAddress(), 0x2001);

        ppu.onCpuWrite(ppuCtrl, 0x04);
        ppu.onCpuWrite(ppuData, 0xA5);

        // Check that the second write is committed and the address increments by 32
        CHECK_EQ(ppuBus.read(0x2001), 0xA5);
        CHECK_EQ(ppu.internalRegisters().v.busAddress(), 0x2021);
    }
}

TEST_CASE("Render timing")
{
    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines{};
    Ppu            ppu(ppuBus, interruptLines);
    ppu.startup();

    // Enable background rendering by default
    ppu.onCpuWrite(kPpuMask, 0x08);

    SUBCASE("when drawing across") {
        SUBCASE("increments v coarseX bits every 8th dot from 1-248") {
            uint64 cycles = 1;
            ppu.executeUntil(cycles);

            for (int i = 0; i < 31; ++i) {
                CAPTURE(i);

                cycles += 7;
                ppu.executeUntil(cycles);

                CHECK_EQ(ppu.internalRegisters().v.coarseX(), i);

                ppu.executeUntil(++cycles);

                CHECK_EQ(ppu.internalRegisters().v.coarseX(), i + 1);
            }
        }
        SUBCASE("wraps around at 31 to 0 at dot 256") {
            ppu.executeUntil(256);
            CHECK_EQ(ppu.internalRegisters().v.coarseX(), 31);

            ppu.executeUntil(257);
            CHECK_EQ(ppu.internalRegisters().v.coarseX(), 0);
        }
        SUBCASE("toggles horizontal nametable bit when wrapping value") {
            ppu.executeUntil(256);
            CHECK_EQ(ppu.internalRegisters().v.nametableIndex(), 0);

            ppu.executeUntil(257);
            CHECK_EQ(ppu.internalRegisters().v.nametableIndex(), 1);
        }
        SUBCASE("copies horizontal bits from t to v at dot 257") {
            ppu.executeUntil(257);
            CHECK_NE(ppu.internalRegisters().v.horizontalBits(), ppu.internalRegisters().t.horizontalBits());

            ppu.executeUntil(258);
            CHECK_EQ(ppu.internalRegisters().v.horizontalBits(), ppu.internalRegisters().t.horizontalBits());
        }
        SUBCASE("does not modify vertical bits of v at dot 257") {
            ppu.executeUntil(257);
            const auto v = ppu.internalRegisters().v;

            ppu.executeUntil(258);
            CHECK_EQ(ppu.internalRegisters().v.verticalBits(), v.verticalBits());
        }
        SUBCASE("does not modify x register") {
            ppu.executeUntil(Ppu::kPpuCyclesPerFrame);
            CHECK_EQ(ppu.internalRegisters().x, 0);
        }
    }

    SUBCASE("when drawing down") {
        SUBCASE("increments fineY at dot 256 if < 7") {
            ppu.executeUntil(256);
            CHECK_EQ(ppu.internalRegisters().v.fineY(), 0);

            ppu.executeUntil(257);
            CHECK_EQ(ppu.internalRegisters().v.fineY(), 1);
        }
        SUBCASE("increments v coarseY bits every 8 scanlines from 0-239 at dot 256") {
            // First line is a special case: 256 + Ppu::kFrameScanlineWidth * 7
            uint64 cycles = 2643;
            ppu.executeUntil(cycles);

            CHECK_EQ(ppu.internalRegisters().v.coarseY(), 0);
            ppu.executeUntil(++cycles);
            CHECK_EQ(ppu.internalRegisters().v.coarseY(), 1);

            for (int i = 1; i < 29; ++i) {
                CAPTURE(i);

                // Advance the scanline to just before the next tile increment
                cycles += Ppu::kFrameScanlineWidth * 8 - 1;
                ppu.executeUntil(cycles);

                CHECK_EQ(ppu.internalRegisters().v.coarseY(), i);

                // Ensure the increment happens on dot 256 of every scanline
                ppu.executeUntil(++cycles);
                CHECK_EQ(ppu.internalRegisters().v.coarseY(), i + 1);
            }
        }
        SUBCASE("wraps around at 29 to 0") {
            uint64 cycles = 81755; // The number of cycles just before the rollover: 239 * 341 + 256
            ppu.executeUntil(cycles);

            CHECK_EQ(ppu.internalRegisters().v.coarseY(), 29);

            ppu.executeUntil(++cycles);

            CHECK_EQ(ppu.internalRegisters().v.coarseY(), 0);
            CHECK_EQ(ppu.internalRegisters().v.fineY(), 0);
        }
        SUBCASE("toggles vertical nametable bit when wrapping value") {
            uint64 cycles = 81755; // The number of cycles just before the rollover: 239 * 341 + 256
            ppu.executeUntil(cycles);

            CHECK_EQ(ppu.internalRegisters().v.nametableIndex(), 0);

            ppu.executeUntil(++cycles);

            // The horizontal nametable flip happens on the same cycle, so check just the vertical nametable bit
            CHECK_EQ(ppu.internalRegisters().v.nametableIndex() & 0x2, 2);
        }
    }

    SUBCASE("during pre-render scanline") {
        SUBCASE("copies vertical bits from t to v during dots 280-304") {
            uint64 cycles = 89281; // The number of cycles just before the vbit copies: 261 * 341 + 280
            ppu.executeUntil(cycles);
            CHECK_NE(ppu.internalRegisters().v.verticalBits(), ppu.internalRegisters().t.verticalBits());

            const auto t = ppu.internalRegisters().t;
            for (int i = 0; i < 25; ++i) {
                ppu.executeUntil(++cycles);
                CHECK_EQ(ppu.internalRegisters().v.verticalBits(), t.verticalBits());
            }
        }
    }

    /*SUBCASE("when rendering disabled") {
        ppu.onCpuWrite(kPpuCtrl, 0x00);

        SUBCASE("does not increment coarseX during visible scanlines") {}
        SUBCASE("does not increment Y at dot 256") {}
        SUBCASE("does not copy t to v at dot 257") {}
        SUBCASE("does not copy t to v during pre-render scanline") {}
    }*/
}

TEST_CASE("Render addressing")
{
    struct Case {
        uint16      inputAddress;
        uint16      expectedAddress;
        const char* name;
    };

    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines{};
    Ppu            ppu(ppuBus, interruptLines);
    ppu.startup();

    SUBCASE("nametable address") {
        SUBCASE("input address matches expected nametable address") {
            constexpr Case cases[] = {
                { 0x0000, 0x2000, "$0000 -> nametable $2000" },
                { 0x1000, 0x2000, "$1000 -> nametable $2000" },
                { 0x2000, 0x2000, "$2000 -> nametable $2000" },
                { 0x2400, 0x2400, "$2400 -> nametable $2400" },
                { 0x2800, 0x2800, "$2800 -> nametable $2800" },
                { 0x2C00, 0x2C00, "$2C00 -> nametable $2C00" },
                { 0x3000, 0x2000, "$3000 -> nametable $2000" },
                { 0x3400, 0x2400, "$3400 -> nametable $2400" },
                { 0x3800, 0x2800, "$3800 -> nametable $2800" },
                { 0x3C00, 0x2C00, "$3C00 -> nametable $2C00" },
            };

            for (const auto& c : cases) {
                SUBCASE(c.name) {
                    ppu.onCpuWrite(kPpuAddr, c.inputAddress >> 8 & 0xFF);
                    ppu.onCpuWrite(kPpuAddr, c.inputAddress & 0xFF);

                    CHECK_EQ(ppu.internalRegisters().v.nametableAddress(), c.expectedAddress);
                }
            }
        }

        SUBCASE("fineY does not affect nametable address") {
            ppu.onCpuWrite(kPpuAddr, 0xFC);
            ppu.onCpuWrite(kPpuAddr, 0x00);

            CHECK_EQ(ppu.internalRegisters().v.nametableAddress(), 0x2C00);
        }
    }

    SUBCASE("attribute address") {
        SUBCASE("input address matches expected nametable address") {
            constexpr Case cases[] = {
                // High address bits ($0xxx/$1xxx) are masked off by PPUADDR; only bits 0-13 reach v
                { 0x0000, 0x23C0, "$0000 -> attribute table $23C0" },
                { 0x1000, 0x23C0, "$1000 -> attribute table $23C0" },

                // coarseX high 3 bits drive attribute bits 0-2: four consecutive tiles share one attribute byte
                { 0x2000, 0x23C0, "$2000 (coarseX=0) -> attribute table $23C0" },
                { 0x2001, 0x23C0, "$2001 (coarseX=1) -> attribute table $23C0" },
                { 0x2002, 0x23C0, "$2002 (coarseX=2) -> attribute table $23C0" },
                { 0x2003, 0x23C0, "$2003 (coarseX=3) -> attribute table $23C0" },
                { 0x2004, 0x23C1, "$2004 (coarseX=4) -> attribute table $23C1" },
                { 0x201F, 0x23C7, "$201F (coarseX=31) -> attribute table $23C7" },

                // coarseY high 3 bits drive attribute bits 3-5: four tile rows share one attribute byte
                { 0x2020, 0x23C0, "$2020 (coarseY=1) -> attribute table $23C0" },
                { 0x203F, 0x23C7, "$203F (coarseY=1,coarseX=31) -> attribute table $23C7" },
                { 0x2040, 0x23C0, "$2040 (coarseY=2) -> attribute table $23C0" },
                { 0x2060, 0x23C0, "$2060 (coarseY=3) -> attribute table $23C0" },
                { 0x207F, 0x23C7, "$207F (coarseY=3,coarseX=31) -> attribute table $23C7" },
                { 0x2080, 0x23C8, "$2080 (coarseY=4) -> attribute table $23C8" },
                { 0x2260, 0x23E0, "$2260 (coarseY=19) -> attribute table $23E0" },
                { 0x23E0, 0x23F8, "$23E0 (coarseY=31) -> attribute table $23F8" },
                { 0x23FF, 0x23FF, "$23FF (coarseY=31,coarseX=31) -> attribute table $23FF" },

                // Nametable select (bits 10-11) passes straight through to attribute bits 10-11
                { 0x2400, 0x27C0, "$2400 (NN=1) -> attribute table $27C0" },
                { 0x2800, 0x2BC0, "$2800 (NN=2) -> attribute table $2BC0" },
                { 0x2C00, 0x2FC0, "$2C00 (NN=3) -> attribute table $2FC0" },
                { 0x2FFF, 0x2FFF, "$2FFF (NN=3, coarseY=31, coarseX=31) -> attribute table $2FFF" },

                // $3000-$3FFF mirrors $2000-$2FFF: bit 12 is fineY and does not reach the attribute address
                { 0x3000, 0x23C0, "$3000 -> attribute table $23C0 (mirrors $2000)" },
                { 0x3400, 0x27C0, "$3400 -> attribute table $27C0 (mirrors $2400)" },
                { 0x3800, 0x2BC0, "$3800 -> attribute table $2BC0 (mirrors $2800)" },
                { 0x3C00, 0x2FC0, "$3C00 -> attribute table $2FC0 (mirrors $2C00)" },
            };

            for (const auto& c : cases) {
                SUBCASE(c.name) {
                    ppu.onCpuWrite(kPpuAddr, c.inputAddress >> 8 & 0xFF);
                    ppu.onCpuWrite(kPpuAddr, c.inputAddress & 0xFF);

                    CHECK_EQ(ppu.internalRegisters().v.attributeTableAddress(), c.expectedAddress);
                }
            }
        }
        SUBCASE("fineY does not affect attribute address") {
            // PPUADDR masks the high byte to 6 bits, so $7C00 lands in v as $3C00:
            // fineY=3 (bit 14 dropped), NN=3, coarseY=0, coarseX=0 -> same attribute byte as $2C00
            ppu.onCpuWrite(kPpuAddr, 0x7C00 >> 8 & 0xFF);
            ppu.onCpuWrite(kPpuAddr, 0x7C00 & 0xFF);

            CHECK_EQ(ppu.internalRegisters().v.attributeTableAddress(), 0x2FC0);
        }
    }

    SUBCASE("pattern address") {
        struct PatternCase {
            bool        tableSelect;
            bool        plane;
            uint8       tileIndex;
            uint8       tileRow;
            uint16      expectedAddress;
            const char* name;
        };

        constexpr PatternCase cases[] = {
            // Baseline: every field zero
            { false, false, 0x00, 0, 0x0000, "all zero -> $0000" },

            // tableSelect routes to bit 12 (which pattern table half)
            { true, false, 0x00, 0, 0x1000, "tableSelect -> bit 12 ($1000)" },

            // tileIndex shifts into bits 4-11
            { false, false, 0x01, 0, 0x0010, "tileIndex $01 -> bit 4 ($0010)" },
            { false, false, 0x0F, 0, 0x00F0, "tileIndex $0F -> $00F0" },
            { false, false, 0x10, 0, 0x0100, "tileIndex $10 -> $0100" },
            { false, false, 0x80, 0, 0x0800, "tileIndex $80 -> $0800" },
            { false, false, 0xFF, 0, 0x0FF0, "tileIndex $FF -> bits 4-11 ($0FF0)" },

            // plane routes to bit 3 (low vs high bit plane of the tile row)
            { false, true, 0x00, 0, 0x0008, "plane -> bit 3 ($0008)" },

            // tileRow routes to bits 0-2 (row within the tile)
            { false, false, 0x00, 1, 0x0001, "tileRow 1 -> $0001" },
            { false, false, 0x00, 7, 0x0007, "tileRow 7 -> bits 0-2 ($0007)" },

            // Adjacency: plane (bit 3) sits directly above tileRow (bits 0-2); together they fill the low nibble
            { false, true, 0x00, 7, 0x000F, "plane + tileRow 7 -> $000F" },

            // Adjacency: tileIndex's low bit (bit 4) sits directly above plane (bit 3)
            { false, true, 0x01, 0, 0x0018, "tileIndex $01 + plane -> $0018" },

            // Adjacency: tileIndex's high bit (bit 11) sits directly below tableSelect (bit 12)
            { true, false, 0xFF, 0, 0x1FF0, "tableSelect + tileIndex $FF -> $1FF0" },

            // All fields saturated -> max pattern address
            { true, true, 0xFF, 7, 0x1FFF, "all set -> $1FFF" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                CHECK_EQ(Ppu::patternTableAddress(c.tableSelect, c.plane, c.tileIndex, c.tileRow), c.expectedAddress);
            }
        }
    }
}

TEST_CASE("Background pixel composition")
{
    SUBCASE("pattern bytes combine into 2-bit color index") {
        SUBCASE("both planes zero produces color 0") {}
        SUBCASE("low plane bit alone produces color 1") {}
        SUBCASE("high plane bit alone produces color 2") {}
        SUBCASE("both planes set produces color 3") {}
        SUBCASE("leftmost pixel selects MSB of pattern bytes") {}
        SUBCASE("rightmost pixel selects LSB of pattern bytes") {}
    }

    SUBCASE("attribute byte yields 2-bit palette select for tile quadrant") {
        SUBCASE("top-left quadrant uses bits 0-1") {}
        SUBCASE("top-right quadrant uses bits 2-3") {}
        SUBCASE("bottom-left quadrant uses bits 4-5") {}
        SUBCASE("bottom-right quadrant uses bits 6-7") {}
    }

    SUBCASE("final palette index stitches palette select with color") {
        SUBCASE("color 0 always maps to backdrop regardless of palette select") {}
        SUBCASE("non-zero color combines with palette select into 4-bit index") {}
    }
}

TEST_CASE("NMI interrupt")
{
    SUBCASE("when enabled triggers on first vBlank scanline") {
        CiRam          ciram;
        PpuBus         ppuBus(ciram);
        InterruptLines interruptLines;
        Ppu            ppu(ppuBus, interruptLines);

        // Ensure NMI is enabled
        ppu.onCpuWrite(0x2000, 0x80);

        ppu.executeUntil(Ppu::kFrameScanlineWidth * 241 + 1);

        CHECK(interruptLines.nmiActive);
    }

    SUBCASE("when disabled does not trigger on first vBlank scanline") {
        CiRam          ciram;
        PpuBus         ppuBus(ciram);
        InterruptLines interruptLines;
        Ppu            ppu(ppuBus, interruptLines);

        // Ensure NMI is disabled
        ppu.onCpuWrite(0x2000, 0x00);

        ppu.executeUntil(Ppu::kFrameScanlineWidth * 241 + 1);

        CHECK_FALSE(interruptLines.nmiActive);
    }
}

TEST_CASE("NAMETABLES")
{
    struct MirroringTestCartridge {
        enum MirrorType {
            kVertical,
            kHorizontal,
        };

        explicit MirroringTestCartridge(const MirrorType type) : m_type(type) {}

        static uint8 onPpuRead(uint16 addr) { return 0; }
        static void  onPpuWrite(uint16 addr, uint8 data) {}

        [[nodiscard]] static bool isCiRamEnabled() { return true; }
        [[nodiscard]] bool        isHorizontalMirrored() const { return m_type == kHorizontal; }

    private:
        MirrorType m_type;
    };

    struct Case {
        uint16      ppuBase;
        uint16      ciramBase;
        const char* name;
    };

    SUBCASE("write vertical mirroring: nametable address maps to expected CIRAM page") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kVertical);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        constexpr Case cases[] = {
            { 0x2000, 0x000, "$2000 -> CIRAM $000" },
            { 0x2400, 0x400, "$2400 -> CIRAM $400" },
            { 0x2800, 0x000, "$2800 -> CIRAM $000 (mirrors $2000)" },
            { 0x2C00, 0x400, "$2C00 -> CIRAM $400 (mirrors $2400)" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                for (uint16 i = 0; i < 0x400; ++i) {
                    CAPTURE(i);
                    ppuBus.write(c.ppuBase + i, static_cast<uint8>(i ^ 0xA5));

                    CHECK_EQ(ciram.read(c.ciramBase + i), static_cast<uint8>(i ^ 0xA5));
                }
            }
        }
    }

    SUBCASE("write horizontal mirroring: nametable address maps to expected CIRAM page") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kHorizontal);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        constexpr Case cases[] = {
            { 0x2000, 0x000, "$2000 -> CIRAM $000" },
            { 0x2400, 0x000, "$2400 -> CIRAM $000 (mirrors $2000)" },
            { 0x2800, 0x400, "$2800 -> CIRAM $400" },
            { 0x2C00, 0x400, "$2C00 -> CIRAM $400 (mirrors $2800)" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                for (uint16 i = 0; i < 0x400; ++i) {
                    CAPTURE(i);
                    ppuBus.write(c.ppuBase + i, static_cast<uint8>(i ^ 0xA5));

                    CHECK_EQ(ciram.read(c.ciramBase + i), static_cast<uint8>(i ^ 0xA5));
                }
            }
        }
    }

    SUBCASE("read vertical mirroring: CIRAM page maps to expected nametable address") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kVertical);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        constexpr Case cases[] = {
            { 0x2000, 0x000, "CIRAM $000 -> $2000" },
            { 0x2400, 0x400, "CIRAM $400 -> $2400" },
            { 0x2800, 0x000, "CIRAM $000 -> $2800 (mirrors $2000)" },
            { 0x2C00, 0x400, "CIRAM $400 -> $2C00 (mirrors $2400)" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                for (uint16 i = 0; i < 0x400; ++i) {
                    CAPTURE(i);
                    ciram.write(c.ciramBase + i, static_cast<uint8>(i ^ 0xA5));

                    CHECK_EQ(ppuBus.read(c.ppuBase + i), static_cast<uint8>(i ^ 0xA5));
                }
            }
        }
    }

    SUBCASE("read horizontal mirroring: CIRAM page maps to expected nametable address") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kHorizontal);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        constexpr Case cases[] = {
            { 0x2000, 0x000, "CIRAM $000 -> $2000" },
            { 0x2400, 0x000, "CIRAM $000 -> $2400 (mirrors $2000)" },
            { 0x2800, 0x400, "CIRAM $400 -> $2800" },
            { 0x2C00, 0x400, "CIRAM $400 -> $2C00 (mirrors $2800)" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                for (uint16 i = 0; i < 0x400; ++i) {
                    CAPTURE(i);
                    ciram.write(c.ciramBase + i, static_cast<uint8>(i ^ 0xA5));

                    CHECK_EQ(ppuBus.read(c.ppuBase + i), static_cast<uint8>(i ^ 0xA5));
                }
            }
        }
    }

    SUBCASE("vertical mirroring: $2000 and $2800 alias; $2000 and $2400 do not") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kVertical);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        ppuBus.write(0x2000, 0xAA);
        ppuBus.write(0x2800, 0x55);
        CHECK_EQ(ppuBus.read(0x2000), 0x55); // second write overwrote first

        ppuBus.write(0x2000, 0x11);
        ppuBus.write(0x2400, 0x22);
        CHECK_EQ(ppuBus.read(0x2000), 0x11); // top vs bottom are independent
        CHECK_EQ(ppuBus.read(0x2400), 0x22);
    }

    SUBCASE("horizontal mirroring: $2000 and $2400 alias; $2000 and $2800 do not") {
        MirroringTestCartridge cartridge(MirroringTestCartridge::kHorizontal);
        CiRam                  ciram;
        PpuBus                 ppuBus(ciram);
        ppuBus.attachCartridge(cartridge);

        ppuBus.write(0x2000, 0xAA);
        ppuBus.write(0x2400, 0x55);
        CHECK_EQ(ppuBus.read(0x2000), 0x55); // second write overwrote first

        ppuBus.write(0x2000, 0x11);
        ppuBus.write(0x2800, 0x22);
        CHECK_EQ(ppuBus.read(0x2000), 0x11); // top vs bottom are independent
        CHECK_EQ(ppuBus.read(0x2800), 0x22);
    }
}
}
