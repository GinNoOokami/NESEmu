#include "NESEmuCore/ppu.hpp"
#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/interrupt_lines.hpp"

#include <doctest.h>

using namespace NESEmu;

constexpr uint16 kPpuCtrl   = 0x2000;
constexpr uint16 kPpuMask   = 0x2001;
constexpr uint16 kOamAddr   = 0x2003;
constexpr uint16 kOamData   = 0x2004;
constexpr uint16 kPpuScroll = 0x2005;
constexpr uint16 kPpuAddr   = 0x2006;
constexpr uint16 kPpuData   = 0x2007;

// $3F00 universal background marker used by the sprite render-path tests.
constexpr uint8 kGlobalBackdropColor = 0x0D;

// Writes one sprite (4 bytes) into primary OAM at the given sprite index via OAMADDR/OAMDATA.
static void writeSprite(Ppu& ppu, const uint8 index, const uint8 y, const uint8 tile, const uint8 attr, const uint8 x)
{
    ppu.onCpuWrite(kOamAddr, static_cast<uint8>(index * 4));
    ppu.onCpuWrite(kOamData, y);
    ppu.onCpuWrite(kOamData, tile);
    ppu.onCpuWrite(kOamData, attr);
    ppu.onCpuWrite(kOamData, x);
}

// Parks every sprite off-screen (Y = $FF) so only sprites written afterward are in range.
static void parkAllSprites(Ppu& ppu)
{
    ppu.onCpuWrite(kOamAddr, 0x00);
    for (int i = 0; i < 256; ++i) {
        ppu.onCpuWrite(kOamData, 0xFF);
    }
}

// Runs the PPU until sprite evaluation for the given display scanline has completed.
// Evaluation for line N runs at master cycle N*341 (the transition into line N), so any
// target in (N*341, (N+1)*341] observes the result; N must be >= 1 in the first frame.
static void evaluateForScanline(Ppu& ppu, const int scanline)
{
    ppu.executeUntil(static_cast<uint64>(scanline) * Ppu::kFrameScanlineWidth + 1);
}

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

            CHECK_EQ(ppu.internalRegisters().v.nametableIndex() & 0x2, 0);

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
                CHECK_EQ(Ppu::PatternTable::address(c.tableSelect, c.plane, c.tileIndex, c.tileRow), c.expectedAddress);
            }
        }
    }
}

TEST_CASE("Background pixel composition")
{
    SUBCASE("pattern bytes combine into 2-bit color index") {
        // dot is screen-x: dot 0 is the leftmost pixel (bit 7), dot 7 the rightmost (bit 0).
        // color = (hiBit << 1) | loBit, where the bit read at dot d is bit (7 - d) of each plane.
        struct Case {
            uint8       bitPlaneLo;
            uint8       bitPlaneHi;
            uint8       expected[8]; // expected color per dot, dot 0 (leftmost) first
            const char* name;
        };

        constexpr Case cases[] = {
            // Uniform planes: same color across the whole row
            { 0x00, 0x00, { 0, 0, 0, 0, 0, 0, 0, 0 }, "both planes clear -> all color 0" },
            { 0xFF, 0x00, { 1, 1, 1, 1, 1, 1, 1, 1 }, "low plane all set -> all color 1" },
            { 0x00, 0xFF, { 2, 2, 2, 2, 2, 2, 2, 2 }, "high plane all set -> all color 2" },
            { 0xFF, 0xFF, { 3, 3, 3, 3, 3, 3, 3, 3 }, "both planes all set -> all color 3" },

            // Single bit: isolates the leftmost (MSB) and rightmost (LSB) pixel positions
            { 0x80, 0x00, { 1, 0, 0, 0, 0, 0, 0, 0 }, "low plane MSB -> color 1 at dot 0 only" },
            { 0x01, 0x00, { 0, 0, 0, 0, 0, 0, 0, 1 }, "low plane LSB -> color 1 at dot 7 only" },
            { 0x00, 0x80, { 2, 0, 0, 0, 0, 0, 0, 0 }, "high plane MSB -> color 2 at dot 0 only" },
            { 0x00, 0x01, { 0, 0, 0, 0, 0, 0, 0, 2 }, "high plane LSB -> color 2 at dot 7 only" },
            { 0x80, 0x80, { 3, 0, 0, 0, 0, 0, 0, 0 }, "both planes MSB -> color 3 at dot 0 only" },
            { 0x01, 0x01, { 0, 0, 0, 0, 0, 0, 0, 3 }, "both planes LSB -> color 3 at dot 7 only" },

            // Mixed planes: every dot combines its two plane bits independently
            { 0xAA, 0xCC, { 3, 2, 1, 0, 3, 2, 1, 0 }, "lo=$AA hi=$CC -> 3,2,1,0 repeating" },
            { 0xCC, 0xAA, { 3, 1, 2, 0, 3, 1, 2, 0 }, "lo=$CC hi=$AA -> 3,1,2,0 repeating" },
            { 0x0F, 0xF0, { 2, 2, 2, 2, 1, 1, 1, 1 }, "lo=$0F hi=$F0 -> left half color 2, right half color 1" },
            { 0x3C, 0x18, { 0, 0, 1, 3, 3, 1, 0, 0 }, "lo=$3C hi=$18 -> 0,0,1,3,3,1,0,0" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                auto tilePattern = Ppu::PatternTable::makeTile(c.bitPlaneLo, c.bitPlaneHi);
                for (uint8 dot = 0; dot < 8; ++dot) {
                    CAPTURE(dot);
                    CHECK_EQ(tilePattern.paletteIndex(dot), c.expected[dot]);
                }
            }
        }
    }

    SUBCASE("attribute byte yields 2-bit palette select for tile quadrant") {
        // The quadrant is chosen by bit 1 of coarseX (left/right half) and bit 1 of coarseY
        // (top/bottom half) of the 4x4-tile attribute region. Quadrant -> bit pair:
        //   top-left  = bits 1-0, top-right    = bits 3-2,
        //   bottom-left = bits 5-4, bottom-right = bits 7-6.
        struct Case {
            uint8       attributeByte;
            uint8       coarseX;
            uint8       coarseY;
            uint8       expected;
            const char* name;
        };

        // 0xE4 = 0b11'10'01'00: each quadrant holds its own index (TL=0,TR=1,BL=2,BR=3)
        // 0x1B = 0b00'01'10'11: reversed, guards against an identity-shift shortcut
        constexpr Case cases[] = {
            // Quadrant selection with the distinct-per-quadrant byte (coarseX/Y bit 1 chooses)
            { 0xE4, 0, 0, 0, "$E4 TL (cX bit1=0, cY bit1=0) -> 0" },
            { 0xE4, 2, 0, 1, "$E4 TR (cX bit1=1, cY bit1=0) -> 1" },
            { 0xE4, 0, 2, 2, "$E4 BL (cX bit1=0, cY bit1=1) -> 2" },
            { 0xE4, 2, 2, 3, "$E4 BR (cX bit1=1, cY bit1=1) -> 3" },

            // Same byte, reversed packing: confirms each quadrant reads its own 2-bit field
            { 0x1B, 0, 0, 3, "$1B TL -> 3" },
            { 0x1B, 2, 0, 2, "$1B TR -> 2" },
            { 0x1B, 0, 2, 1, "$1B BL -> 1" },
            { 0x1B, 2, 2, 0, "$1B BR -> 0" },

            // Only bit 1 of coarse position matters: bit 0 must not change the quadrant.
            // coarseX 0 and 1 are the same quadrant column; 2 and 3 are the next.
            { 0xE4, 1, 0, 0, "$E4 cX=1 (bit0 set) stays in TL -> 0" },
            { 0xE4, 3, 0, 1, "$E4 cX=3 stays in TR -> 1" },
            { 0xE4, 0, 1, 0, "$E4 cY=1 (bit0 set) stays in TL -> 0" },
            { 0xE4, 0, 3, 2, "$E4 cY=3 stays in BL -> 2" },

            // High bits of coarseX/Y (>=4, the next attribute region) must not leak into the
            // quadrant select: bit 2 and above are masked away by the lookup.
            { 0xE4, 4, 0, 0, "$E4 cX=4 (bit2 set) -> TL, not affected -> 0" },
            { 0xE4, 6, 0, 1, "$E4 cX=6 (bits 2+1) -> TR -> 1" },
            { 0xE4, 0, 4, 0, "$E4 cY=4 -> TL -> 0" },
            { 0xE4, 0, 6, 2, "$E4 cY=6 -> BL -> 2" },
            { 0xE4, 31, 31, 3, "$E4 cX=31 cY=31 (max coarse) -> BR -> 3" },

            // Uniform byte: every quadrant yields the same value
            { 0xFF, 0, 0, 3, "$FF TL -> 3" },
            { 0xFF, 2, 2, 3, "$FF BR -> 3" },
            { 0x00, 2, 2, 0, "$00 BR -> 0" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                CHECK_EQ(Ppu::selectPaletteFromAttribute(c.attributeByte, c.coarseX, c.coarseY), c.expected);
            }
        }
    }

    SUBCASE("final palette index stitches palette select with color") {
        // Drives a real render of a single tile on scanline 0 and reads the composed pixel
        // back through the visible frame buffer. Pattern data is served by a CHR cartridge;
        // nametable/attribute bytes live in CIRAM. The composition rule under test:
        //   color 0  -> palette index 0 ($3F00 backdrop), palette select ignored
        //   color !=0 -> (paletteSelect << 2) | color
        struct PatternTestCartridge {
            // Pattern table half 0: tile 0 occupies $0000-$000F (lo plane $0000-7, hi plane $0008-F)
            uint8 planeLo{};
            uint8 planeHi{};

            uint8 onPpuRead(const uint16 addr) const
            {
                if (addr <= 0x0007)
                    return planeLo; // every row of tile 0 returns the same lo byte
                if (addr <= 0x000F)
                    return planeHi;
                return 0;
            }

            static void onPpuWrite(uint16, uint8) {}

            [[nodiscard]] static bool isCiRamEnabled() { return true; }
            [[nodiscard]] static bool isHorizontalMirrored() { return false; }
        };

        constexpr uint16 ppuAddr = 0x2006;
        constexpr uint16 ppuData = 0x2007;
        constexpr uint16 ppuMask = 0x2001;

        // Distinct palette RAM markers so a wrong index is unambiguous.
        // Index:  0    1    2    3     4    5    6    7     ...  15
        //        bg0  ...                                        bg3-c3
        constexpr uint8 backdrop = 0x21; // $3F00 universal background (e.g. SMB sky blue)

        struct Case {
            uint8       planeLo; // low bit-plane byte for the tile row
            uint8       planeHi; // high bit-plane byte
            uint8       attributeByte; // selects the palette for the rendered quadrant (TL here)
            uint8       leftPixel; // expected palette VALUE at dot 0 (bit 7 of the planes)
            const char* name;
        };

        // Palette RAM is filled below as m_paletteData[i] = 0x30 + i for i in 1..15, backdrop at 0.
        // So a non-zero composed index N is expected to read back as 0x30 + N.
        const auto expectedValueForIndex = [&](const uint8 index) -> uint8 {
            return index == 0 ? backdrop : static_cast<uint8>(0x30 + index);
        };

        const Case cases[] = {
            // color 0 at dot 0 -> backdrop regardless of palette select (attribute byte)
            { 0x00, 0x00, 0x00, backdrop, "color 0, select 0 -> backdrop" },
            { 0x00, 0x00, 0x01, backdrop, "color 0, select 1 -> backdrop (select ignored)" },
            { 0x00, 0x00, 0x03, backdrop, "color 0, select 3 -> backdrop (select ignored)" },

            // non-zero color at dot 0 (bit 7 set) -> (select << 2) | color
            { 0x80, 0x00, 0x00, expectedValueForIndex(1), "color 1, select 0 -> index 1" },
            { 0x00, 0x80, 0x00, expectedValueForIndex(2), "color 2, select 0 -> index 2" },
            { 0x80, 0x80, 0x00, expectedValueForIndex(3), "color 3, select 0 -> index 3" },
            { 0x80, 0x00, 0x01, expectedValueForIndex(5), "color 1, select 1 -> index 5" },
            { 0x00, 0x80, 0x01, expectedValueForIndex(6), "color 2, select 1 -> index 6" },
            { 0x80, 0x80, 0x03, expectedValueForIndex(15), "color 3, select 3 -> index 15" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                PatternTestCartridge cartridge{ c.planeLo, c.planeHi };
                CiRam                ciram;
                PpuBus               ppuBus(ciram);
                InterruptLines       interruptLines{};
                Ppu                  ppu(ppuBus, interruptLines);
                ppuBus.attachCartridge(cartridge);
                ppu.startup();

                // Fill the background palettes only ($3F00-$3F0F): backdrop at $3F00, distinct
                // markers 0x31..0x3F at $3F01..$3F0F. We deliberately stop at $3F0F so the fill
                // never touches $3F10/14/18/1C, which mirror down onto $3F00/04/08/0C and would
                // otherwise clobber the values we just wrote.
                ppu.onCpuWrite(ppuAddr, 0x3F);
                ppu.onCpuWrite(ppuAddr, 0x00);
                ppu.onCpuWrite(ppuData, backdrop);
                for (uint8 i = 1; i < 0x10; ++i) {
                    ppu.onCpuWrite(ppuData, static_cast<uint8>(0x30 + i));
                }

                // Tile 0 at nametable $2000; attribute byte at $23C0 (covers the top-left quadrant)
                ppuBus.write(0x2000, 0x00);
                ppuBus.write(0x23C0, c.attributeByte);

                // Point v at coarseX=0, coarseY=0, nametable 0 so the tile renders at pixels 0-7, TL quadrant
                ppu.onCpuWrite(ppuAddr, 0x20);
                ppu.onCpuWrite(ppuAddr, 0x00);

                // Enable background rendering and advance far enough to fetch+compose tile 0 of scanline 0
                ppu.onCpuWrite(ppuMask, 0x08);

                // Preload the shift registers for the first two tiles without needing to wait a whole frame
                ppu.preloadShiftRegisters();
                ppu.preloadShiftRegisters();

                ppu.executeUntil(9);
                ppu.updateVisibleFrameBuffer();

                CHECK_EQ(ppu.frameBuffer()[0], c.leftPixel);
            }
        }
    }
}

// --- Background fine-X scrolling -------------------------------------------------------------
//
// These exercise horizontal fine-X scroll, which the tile-batched render path does not yet model.
// They are written against the hardware shift-register behavior and are expected to FAIL until the
// per-pixel background pipeline is in place: two 16-bit pattern shifters (current tile in the low
// byte, the prefetched next tile in the high byte) tapped by a fine-X mux, with the next tile
// fetched one tile ahead so a boundary crossing always has valid data on both sides.
//
// Model under test: with coarse-X = cx and fine-X = fx, screen pixel p shows nametable source pixel
// (cx*8 + fx + p). A marker -- a single opaque background column at absolute source pixel S --
// therefore lands at screen x = S - (cx*8 + fx). When fx > 0, source pixels belonging to the *next*
// tile slide left of the 8-pixel boundary, which can only be drawn if that tile was prefetched.
// See https://www.nesdev.org/wiki/PPU_rendering#Cycles_1-256

// Serves one configurable marker tile (background pattern half 0): the chosen tile index renders a
// single column as color 1, every other column/tile transparent. The same plane bytes are returned
// for every row, so the rendered color is independent of fine/coarse Y -- vertical scroll is moot.
struct BgMarkerCartridge {
    uint8 tile{};
    uint8 planeLo{};
    uint8 planeHi{};

    [[nodiscard]] uint8 onPpuRead(const uint16 addr) const
    {
        if (addr >= 0x1000) // only background pattern half 0 is populated
            return 0;
        if (((addr >> 4) & 0xFF) != tile)
            return 0;
        return (addr & 0x8) ? planeHi : planeLo; // identical byte for every row of the tile
    }

    static void               onPpuWrite(uint16, uint8) {}
    [[nodiscard]] static bool isCiRamEnabled() { return true; }
    [[nodiscard]] static bool isHorizontalMirrored() { return false; }
};

constexpr uint8 kBgMarkerColor  = 0x31; // $3F01: background palette 0, color 1
constexpr int   kScrollTestLine = 100; // arbitrary mid-frame visible line

// Builds a fresh background-only scene with a marker in tile `markerTile` at in-tile column
// `markerCol`, applies (coarseX, fineX) scroll via $2005, renders line `kScrollTestLine` of the
// first frame, and returns the composed pixel at screen position x.
//
// Only horizontal scroll is under test, and v's horizontal bits (coarseX + horizontal nametable)
// are reloaded from t at dot 257 of every visible scanline -- so a scroll written before frame 0
// takes effect immediately, with no need to wait a frame for a vertical t->v latch. The marker tile
// renders an identical row for every fineY, so v's vertical drift across the frame is irrelevant.
// Every nametable cell (all four pages) is set to tile index (addr & 0x1F) = its coarseX, so the
// fetched tile equals the fetch's coarseX no matter which page coarseX-wrap lands v on mid-scanline.
static uint8 renderScrolledMarkerPixel(const uint8 coarseX, const uint8 fineX, const uint8 markerTile, const uint8 markerCol, const uint8 x)
{
    BgMarkerCartridge chr;
    chr.tile    = markerTile;
    chr.planeLo = static_cast<uint8>(1u << (7 - markerCol)); // color 1 at one in-tile column (col 0 = bit 7)
    chr.planeHi = 0;

    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines;
    Ppu            ppu(ppuBus, interruptLines);
    ppuBus.attachCartridge(chr);

    ppu.startup();
    parkAllSprites(ppu);
    ppu.onCpuWrite(kPpuCtrl, 0x00); // background pattern half 0, base nametable $2000

    // Palette: backdrop + a single bright marker for background palette 0, color 1.
    ppu.onCpuWrite(kPpuAddr, 0x3F);
    ppu.onCpuWrite(kPpuAddr, 0x00);
    ppu.onCpuWrite(kPpuData, kGlobalBackdropColor);
    ppu.onCpuWrite(kPpuData, kBgMarkerColor);

    // Map every nametable cell's tile index to its coarseX (addr & 0x1F), across all four pages, so
    // tile T renders at source pixels [8T, 8T+7] on every row -- regardless of which nametable page
    // coarseX-wrap or vertical drift lands v on.
    for (uint16 addr = 0x2000; addr < 0x3000; ++addr) {
        ppuBus.write(addr, static_cast<uint8>(addr & 0x1F));
    }
    // Clear every attribute byte so the palette select is always 0. These position tests assert the
    // marker reads back as the palette-0 color, so a stray non-zero select would point the lookup at
    // an unwritten palette entry and the assertion would break once fine X is implemented. (The
    // attribute-fine-X subcases below set the attribute table deliberately instead.)
    for (uint16 page = 0x2000; page < 0x3000; page += 0x400) {
        for (uint16 off = 0x3C0; off < 0x400; ++off) {
            ppuBus.write(page + off, 0x00);
        }
    }

    ppu.onCpuWrite(kPpuMask, 0x0A); // background enabled + background left-column shown

    // Scroll via $2005: first write carries coarseX (high 5 bits) and fineX (low 3); Y = 0.
    ppu.onCpuWrite(kPpuScroll, static_cast<uint8>(coarseX << 3 | (fineX & 0x7)));
    ppu.onCpuWrite(kPpuScroll, 0x00);

    const uint64 target = static_cast<uint64>(kScrollTestLine) * Ppu::kFrameScanlineWidth + 257;
    ppu.executeUntil(target);
    ppu.updateVisibleFrameBuffer();

    return ppu.frameBuffer()[kScrollTestLine * kScreenDotWidth + x];
}

// Select-1 and select-2 markers for the attribute subcases: color 1 under palette select 1 reads
// $3F05, under select 2 reads $3F09. Distinct values, so a composed pixel reveals which attribute
// region's palette traveled with it.
constexpr uint8 kBgAttrMarkerA = 0x25; // palette select 1, color 1 -> $3F05
constexpr uint8 kBgAttrMarkerB = 0x29; // palette select 2, color 1 -> $3F09

// Like renderScrolledMarkerPixel, but the scene varies the palette *select* with horizontal position
// instead of holding it at 0: every attribute byte is 0x99 (all four quadrant fields = TL/BL:1,
// TR/BR:2), so a tile's select is (coarseX bit 1) ? 2 : 1 -- independent of coarseY drift -- and
// alternates every two tiles (16 source pixels). Source pixel S therefore carries select
// ((S >> 4) & 1) ? 2 : 1, reading back as kBgAttrMarkerB or kBgAttrMarkerA. This exercises the
// attribute shift register staying in lockstep with the pattern bits as fine X drags a pixel across
// a 16-pixel attribute-region boundary -- the analog of the pattern prefetch, for palette select.
static uint8 renderScrolledAttributePixel(const uint8 coarseX, const uint8 fineX, const uint8 markerTile, const uint8 markerCol, const uint8 x)
{
    BgMarkerCartridge chr;
    chr.tile    = markerTile;
    chr.planeLo = static_cast<uint8>(1u << (7 - markerCol)); // color 1 at one in-tile column
    chr.planeHi = 0;

    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines;
    Ppu            ppu(ppuBus, interruptLines);
    ppuBus.attachCartridge(chr);

    ppu.startup();
    parkAllSprites(ppu);
    ppu.onCpuWrite(kPpuCtrl, 0x00); // background pattern half 0, base nametable $2000

    // Background palettes $3F00-$3F0F: backdrop, then 0x21..0x2F so select 1 color 1 ($3F05) reads
    // kBgAttrMarkerA and select 2 color 1 ($3F09) reads kBgAttrMarkerB. Stop at $3F0F so the fill
    // never touches the $3F10/14/18/1C mirrors of the backdrop slots.
    ppu.onCpuWrite(kPpuAddr, 0x3F);
    ppu.onCpuWrite(kPpuAddr, 0x00);
    ppu.onCpuWrite(kPpuData, kGlobalBackdropColor);
    for (uint8 i = 1; i < 0x10; ++i) {
        ppu.onCpuWrite(kPpuData, static_cast<uint8>(0x20 + i));
    }

    // Nametable tile index = coarseX (all four pages), as in renderScrolledMarkerPixel.
    for (uint16 addr = 0x2000; addr < 0x3000; ++addr) {
        ppuBus.write(addr, static_cast<uint8>(addr & 0x1F));
    }
    // Attribute bytes = 0x99 across all four pages: left quadrant -> select 1, right -> select 2.
    for (uint16 page = 0x2000; page < 0x3000; page += 0x400) {
        for (uint16 off = 0x3C0; off < 0x400; ++off) {
            ppuBus.write(page + off, 0x99);
        }
    }

    ppu.onCpuWrite(kPpuMask, 0x0A); // background enabled + background left-column shown

    ppu.onCpuWrite(kPpuScroll, static_cast<uint8>(coarseX << 3 | (fineX & 0x7)));
    ppu.onCpuWrite(kPpuScroll, 0x00);

    const uint64 target = static_cast<uint64>(kScrollTestLine) * Ppu::kFrameScanlineWidth + 257;
    ppu.executeUntil(target);
    ppu.updateVisibleFrameBuffer();

    return ppu.frameBuffer()[kScrollTestLine * kScreenDotWidth + x];
}

TEST_CASE("Background fine X scrolling")
{
    SUBCASE("fine X slides a pixel left within its tile (no boundary crossing)") {
        // Marker at tile 0, column 7 (source pixel 7). screen = 7 - fineX stays inside the first
        // on-screen tile, so this isolates the fine-X mux without depending on the prefetch.
        for (uint8 fineX = 0; fineX <= 7; ++fineX) {
            CAPTURE(fineX);
            const uint8 expectedX = static_cast<uint8>(7 - fineX);
            CHECK_EQ(renderScrolledMarkerPixel(0, fineX, /*tile*/ 0, /*col*/ 7, expectedX), kBgMarkerColor);
            if (expectedX > 0) {
                CHECK_EQ(renderScrolledMarkerPixel(0, fineX, 0, 7, static_cast<uint8>(expectedX - 1)), kGlobalBackdropColor);
            }
        }
    }

    SUBCASE("fine X pulls the next tile's pixel into the first on-screen tile (prefetch)") {
        // Source pixel 8 is column 0 of nametable tile 1. With coarseX = 0 it sits at screen
        // x = 8 - fineX, so any fineX > 0 drags it left of the 8-pixel boundary -- a pixel that can
        // only be drawn if tile 1 was fetched one tile ahead of the visible tile.
        CHECK_EQ(renderScrolledMarkerPixel(0, 0, /*tile*/ 1, /*col*/ 0, 8), kBgMarkerColor); // baseline, no crossing
        for (uint8 fineX = 1; fineX <= 7; ++fineX) {
            CAPTURE(fineX);
            const uint8 expectedX = static_cast<uint8>(8 - fineX);
            CHECK_EQ(renderScrolledMarkerPixel(0, fineX, 1, 0, expectedX), kBgMarkerColor);
            CHECK_EQ(renderScrolledMarkerPixel(0, fineX, 1, 0, static_cast<uint8>(expectedX + 1)), kGlobalBackdropColor);
        }
    }

    SUBCASE("coarse X selects the first tile and fine X offsets within it") {
        // coarseX = 5 makes nametable tile 5 the leftmost on-screen tile; fineX = 3 shifts 3 pixels
        // into it. Source pixel 45 (tile 5, column 5) lands at screen x = 45 - (5*8 + 3) = 2.
        CHECK_EQ(renderScrolledMarkerPixel(5, 3, /*tile*/ 5, /*col*/ 5, 2), kBgMarkerColor);
        CHECK_EQ(renderScrolledMarkerPixel(5, 3, 5, 5, 3), kGlobalBackdropColor); // sharp: neighbor is blank
    }

    SUBCASE("with coarse + fine X, the tile after the first on-screen tile is prefetched") {
        // Same scroll (coarseX = 5, fineX = 3). Tile 6 is the second on-screen tile; its column-0
        // pixel (source 48) lands at screen x = 48 - (5*8 + 3) = 5, proving the tile past the first
        // visible one is fetched ahead even under a non-zero coarse scroll.
        CHECK_EQ(renderScrolledMarkerPixel(5, 3, /*tile*/ 6, /*col*/ 0, 5), kBgMarkerColor);
        CHECK_EQ(renderScrolledMarkerPixel(5, 3, 6, 0, 6), kGlobalBackdropColor);
    }

    // --- Attribute (palette select) under fine X ---------------------------------------------
    // Palette select alternates every two tiles (every 16 source pixels): source pixels 0-15 -> 1,
    // 16-31 -> 2, 32-47 -> 1, 48-63 -> 2, ... A marker at source S reads kBgAttrMarkerA when its
    // region select is 1 and kBgAttrMarkerB when it is 2. Fine X must carry each pixel's own select
    // with it, exactly like the pattern bits, so the readback color reveals whether the attribute
    // shifter stayed in lockstep.

    SUBCASE("fine X keeps a pixel's palette select within its attribute region") {
        // No boundary crossing: the shifted pixel stays inside one 16-pixel region, so its select
        // must not change. Source 7 sits in the select-1 region; source 24 in the select-2 region.
        for (uint8 fineX = 0; fineX <= 7; ++fineX) {
            CAPTURE(fineX);
            CHECK_EQ(renderScrolledAttributePixel(0, fineX, 0, 7, static_cast<uint8>(7 - fineX)), kBgAttrMarkerA);
            CHECK_EQ(renderScrolledAttributePixel(0, fineX, 3, 0, static_cast<uint8>(24 - fineX)), kBgAttrMarkerB);
        }
    }

    SUBCASE("fine X drags a pixel across an attribute-region boundary, carrying the new select") {
        // Source 16 is the first pixel of the select-2 region. With coarseX = 0 it lands at screen
        // x = 16 - fineX, so any fineX > 0 pulls it left of the boundary into screen columns the
        // batched path draws from the select-1 tile -- yet it must still read back as select 2.
        CHECK_EQ(renderScrolledAttributePixel(0, 0, /*tile*/ 2, /*col*/ 0, 16), kBgAttrMarkerB); // baseline, no crossing
        for (uint8 fineX = 1; fineX <= 7; ++fineX) {
            CAPTURE(fineX);
            CHECK_EQ(renderScrolledAttributePixel(0, fineX, 2, 0, static_cast<uint8>(16 - fineX)), kBgAttrMarkerB);
        }
    }

    SUBCASE("coarse + fine X: pixels straddling a region boundary keep their own selects") {
        // coarseX = 5, fineX = 3. Sources 47 and 48 are adjacent but straddle the 48-pixel region
        // boundary: 47 is select 1 (region 32-47), 48 is select 2 (region 48-63). They land at
        // screen x = 47 - 43 = 4 and x = 48 - 43 = 5, and must retain their distinct selects.
        CHECK_EQ(renderScrolledAttributePixel(5, 3, /*tile*/ 5, /*col*/ 7, 4), kBgAttrMarkerA);
        CHECK_EQ(renderScrolledAttributePixel(5, 3, /*tile*/ 6, /*col*/ 0, 5), kBgAttrMarkerB);
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

TEST_CASE("Sprite evaluation (every scanline)")
{
    CiRam          ciram;
    PpuBus         ppuBus(ciram);
    InterruptLines interruptLines;
    Ppu            ppu(ppuBus, interruptLines);
    ppu.startup();

    // Enable background + sprite rendering. Evaluation currently runs regardless of the mask,
    // but enabling it keeps these tests valid if evaluation is later gated on rendering-enabled.
    ppu.onCpuWrite(kPpuMask, 0x18);
    parkAllSprites(ppu);

    SUBCASE("secondary OAM is cleared to $FF when no sprite is in range") {
        evaluateForScanline(ppu, 50);

        for (int i = 0; i < 32; ++i) {
            CAPTURE(i);
            CHECK_EQ(ppu.oamBuffer().raw[i], 0xFF);
        }
    }

    SUBCASE("in-range selection at scanline boundaries (8x8)") {
        // OAM Y holds (top - 1): a sprite with Y=y occupies display lines y+1 .. y+8 (rows 0..7).
        // Evaluation for display line D selects the sprite iff 0 <= (D - y - 1) < 8.
        constexpr uint8 y = 100;

        struct Case {
            int         scanline;
            bool        expectInRange;
            const char* name;
        };

        constexpr Case cases[] = {
            { y, false, "display line == Y (row -1) is out of range" },
            { y + 1, true, "display line Y+1 (row 0, top) is in range" },
            { y + 8, true, "display line Y+8 (row 7, bottom) is in range" },
            { y + 9, false, "display line Y+9 (row 8) is out of range" },
        };

        for (const auto& c : cases) {
            SUBCASE(c.name) {
                writeSprite(ppu, 0, y, 0x42, 0x00, 50);
                evaluateForScanline(ppu, c.scanline);

                if (c.expectInRange) {
                    CHECK_EQ(ppu.oamBuffer().data[0].y, y);
                    CHECK_EQ(ppu.oamBuffer().data[0].tile, 0x42);
                } else {
                    CHECK_EQ(ppu.oamBuffer().raw[0], 0xFF); // slot 0 still empty
                }
            }
        }
    }

    SUBCASE("an in-range sprite is copied with all four bytes intact") {
        writeSprite(ppu, 0, 45, 0x42, 0xE3, 0x77);
        evaluateForScanline(ppu, 50); // Y=45 -> row 4, in range

        const auto& s = ppu.oamBuffer().data[0];
        CHECK_EQ(s.y, 45);
        CHECK_EQ(s.tile, 0x42);
        CHECK_EQ(s.attributes, 0xE3);
        CHECK_EQ(s.x, 0x77);
    }

    SUBCASE("copies in-range sprites into the buffer in primary-OAM scan order") {
        // Sprites at indices 2 and 5 are in range; all others remain parked off-screen.
        writeSprite(ppu, 2, 45, 0xAA, 0x00, 10);
        writeSprite(ppu, 5, 45, 0xBB, 0x00, 20);
        evaluateForScanline(ppu, 50);

        CHECK_EQ(ppu.oamBuffer().data[0].tile, 0xAA); // OAM index 2 -> buffer slot 0
        CHECK_EQ(ppu.oamBuffer().data[1].tile, 0xBB); // OAM index 5 -> buffer slot 1
        CHECK_EQ(ppu.oamBuffer().raw[8], 0xFF); // buffer slot 2 still empty
    }

    SUBCASE("eight in-range sprites fill the buffer") {
        for (uint8 i = 0; i < 8; ++i) {
            writeSprite(ppu, i, 45, /*tile*/ i, 0x00, static_cast<uint8>(i * 8));
        }
        evaluateForScanline(ppu, 50);

        for (uint8 i = 0; i < 8; ++i) {
            CAPTURE(i);
            CHECK_EQ(ppu.oamBuffer().data[i].tile, i);
        }
    }

    SUBCASE("a ninth in-range sprite is not copied into the buffer") {
        for (uint8 i = 0; i < 9; ++i) {
            writeSprite(ppu, i, 45, /*tile*/ i, 0x00, 0);
        }
        evaluateForScanline(ppu, 50);

        // Only the first 8 sprites (tiles 0..7, in scan order) occupy the buffer.
        for (uint8 i = 0; i < 8; ++i) {
            CAPTURE(i);
            CHECK_EQ(ppu.oamBuffer().data[i].tile, i);
        }
        // NOTE: oamBuffer() only exposes 8 slots, so this guards the visible result. The
        // current bounds check writes the 9th entry out of bounds (count > 8 breaks AFTER
        // the write); that overflow is caught by sanitizers, not by this assertion.
    }

    // Deferred (8x16 sprites out of first-pass scope; evaluation hardcodes height 8):
    //   SUBCASE("8x16 sprite is in range for rows 0-15")
    //   SUBCASE("8x16 sprite one past row 15 is out of range")
    // Deferred (accurate sprite-overflow flag has buggy hardware behavior):
    //   SUBCASE("sprite overflow flag is set when a 9th in-range sprite is found")
}

// Serves pattern bytes for tile 0 of each half: sprites from half 0 ($0000-$000F), background
// from half 1 ($1000-$100F); every other fetch returns 0. One configurable byte per row per
// plane, so a tile can encode the fetched row into the rendered color and make row selection,
// flips, and background/sprite muxing observable through the frame buffer. Background planes
// default to 0 (transparent), so tests that only exercise sprites are unaffected.
struct SpriteChrCartridge {
    std::array<uint8, 8> planeLo{}; // sprite tile 0, pattern half 0 ($0000-$000F)
    std::array<uint8, 8> planeHi{};
    std::array<uint8, 8> bgPlaneLo{}; // background tile 0, pattern half 1 ($1000-$100F)
    std::array<uint8, 8> bgPlaneHi{};

    [[nodiscard]] uint8 onPpuRead(const uint16 addr) const
    {
        const uint16 tile = addr & 0x1FF0;
        const uint8  row  = addr & 0x7;
        const bool   hi   = addr & 0x8;
        if (tile == 0x0000) // sprite tile 0 of half 0
            return hi ? planeHi[row] : planeLo[row];
        if (tile == 0x1000) // background tile 0 of half 1
            return hi ? bgPlaneHi[row] : bgPlaneLo[row];
        return 0;
    }

    static void               onPpuWrite(uint16, uint8) {}
    [[nodiscard]] static bool isCiRamEnabled() { return true; }
    [[nodiscard]] static bool isHorizontalMirrored() { return false; }
};

// Tile where every row is a uniform color equal to (row & 3): planeLo carries color bit 0,
// planeHi carries color bit 1. Renders a wrong-row fetch as a wrong color.
static void setRowEncodedTile(SpriteChrCartridge& chr)
{
    for (uint8 r = 0; r < 8; ++r) {
        chr.planeLo[r] = (r & 1) ? 0xFF : 0x00;
        chr.planeHi[r] = (r & 2) ? 0xFF : 0x00;
    }
}

// Sprite-palette-0 markers: color 1/2/3 -> $3F11/$3F12/$3F13 -> 0x21/0x22/0x23. Color 0 is
// transparent and resolves to the backdrop. (Sprite palette 1 color 1 -> $3F15 -> 0x25.)
static uint8 markerForColor(const uint8 color) { return color == 0 ? kGlobalBackdropColor : static_cast<uint8>(0x20 + color); }

// Background-palette-0 markers: color 1/2/3 -> $3F01/$3F02/$3F03 -> 0x11/0x12/0x13. Distinct
// from the sprite markers so a composed pixel reveals which layer won the mux.
static uint8 bgMarkerForColor(const uint8 color) { return color == 0 ? kGlobalBackdropColor : static_cast<uint8>(0x10 + color); }

// Fills background tile 0 (pattern half 1) so every row is color 1 -> an opaque background everywhere.
static void setOpaqueBackground(SpriteChrCartridge& chr) { chr.bgPlaneLo.fill(0xFF); }

// Fills sprite tile 0 (pattern half 0) so every row is color 1 -> a fully opaque sprite.
static void setOpaqueSpriteTile(SpriteChrCartridge& chr) { chr.planeLo.fill(0xFF); }

static void setupSpriteScene(Ppu& ppu)
{
    ppu.startup();

    // Background pattern table -> $1000, sprite pattern table -> $0000 (where the fixture serves
    // each tile 0); 8x8 sprites. Background planes default to 0, so the background is transparent
    // until a test opts into an opaque tile via setOpaqueBackground().
    ppu.onCpuWrite(kPpuCtrl, 0x10);
    parkAllSprites(ppu);

    // Palette RAM markers (palette select 0 unless noted):
    //   $3F00 backdrop; $3F01-$3F03 background; $3F11-$3F13 sprite pal 0; $3F15 sprite pal 1.
    ppu.onCpuWrite(kPpuAddr, 0x3F);
    ppu.onCpuWrite(kPpuAddr, 0x00);
    ppu.onCpuWrite(kPpuData, kGlobalBackdropColor);
    ppu.onCpuWrite(kPpuData, 0x11);
    ppu.onCpuWrite(kPpuData, 0x12);
    ppu.onCpuWrite(kPpuData, 0x13);
    ppu.onCpuWrite(kPpuAddr, 0x3F);
    ppu.onCpuWrite(kPpuAddr, 0x11);
    ppu.onCpuWrite(kPpuData, 0x21);
    ppu.onCpuWrite(kPpuData, 0x22);
    ppu.onCpuWrite(kPpuData, 0x23);
    ppu.onCpuWrite(kPpuData, 0x00); // $3F14 (unused)
    ppu.onCpuWrite(kPpuData, 0x25); // $3F15: sprite palette 1, color 1

    // Enable background + sprite rendering (left-column masks off; tests that care set them).
    ppu.onCpuWrite(kPpuMask, 0x18);
}

// Renders the full visible portion of the given scanline and returns the composed pixel at x.
// Scanlines must be requested in non-decreasing order within a subcase (executeUntil is cumulative).
static uint8 renderScanlinePixel(Ppu& ppu, const int scanline, const uint8 x)
{
    ppu.executeUntil(static_cast<uint64>(scanline) * Ppu::kFrameScanlineWidth + 257);
    ppu.updateVisibleFrameBuffer();
    return ppu.frameBuffer()[scanline * kScreenDotWidth + x];
}

// Renders through the end of the given scanline and reports whether the sprite-0-hit flag is set.
static bool renderAndReadSpriteZeroHit(Ppu& ppu, const int scanline)
{
    ppu.executeUntil(static_cast<uint64>(scanline) * Ppu::kFrameScanlineWidth + 257);
    return ppu.ppuStatus().spriteZeroHit();
}

TEST_CASE("Sprite pattern fetching")
{
    // A sprite with OAM Y=y occupies display lines y+1 .. y+8 (rows 0..7); the fetched
    // pattern row for display line D is (D - y - 1). The row-encoded tile makes the rendered
    // color equal that fetched row & 3, so the frame buffer reveals which row was fetched.
    constexpr uint8 y       = 100;
    constexpr uint8 spriteX = 16; // clear of any left-column edge effects

    SpriteChrCartridge chr;
    CiRam              ciram;
    PpuBus             ppuBus(ciram);
    InterruptLines     interruptLines;
    Ppu                ppu(ppuBus, interruptLines);
    ppuBus.attachCartridge(chr);

    SUBCASE("fetches the pattern row matching (scanline - Y - 1)") {
        setRowEncodedTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        // Display line y+1+r -> row r -> color (r & 3).
        CHECK_EQ(renderScanlinePixel(ppu, y + 1, spriteX), markerForColor(0)); // row 0 -> transparent
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(1)); // row 1
        CHECK_EQ(renderScanlinePixel(ppu, y + 3, spriteX), markerForColor(2)); // row 2
        CHECK_EQ(renderScanlinePixel(ppu, y + 4, spriteX), markerForColor(3)); // row 3
    }

    SUBCASE("vertical flip (attribute bit 7) reads pattern row (height-1 - row)") {
        setRowEncodedTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x80, spriteX);

        // Display line y+1+r -> logical row r -> fetched pattern row (7 - r) -> color ((7-r) & 3).
        CHECK_EQ(renderScanlinePixel(ppu, y + 1, spriteX), markerForColor(3)); // row 0 -> fetch 7
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(2)); // row 1 -> fetch 6
        CHECK_EQ(renderScanlinePixel(ppu, y + 3, spriteX), markerForColor(1)); // row 2 -> fetch 5
        CHECK_EQ(renderScanlinePixel(ppu, y + 4, spriteX), markerForColor(0)); // row 3 -> fetch 4
    }

    SUBCASE("horizontal flip (attribute bit 6) reverses pixel order within the row") {
        // Opaque (color 1) at the leftmost column only; transparent elsewhere.
        for (uint8 r = 0; r < 8; ++r) {
            chr.planeLo[r] = 0x80;
            chr.planeHi[r] = 0x00;
        }
        setupSpriteScene(ppu);

        SUBCASE("not flipped: opaque pixel stays at the left edge") {
            writeSprite(ppu, 0, y, 0, 0x00, spriteX);
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX + 0), markerForColor(1));
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX + 7), kGlobalBackdropColor);
        }
        SUBCASE("flipped: opaque pixel moves to the right edge") {
            writeSprite(ppu, 0, y, 0, 0x40, spriteX);
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX + 0), kGlobalBackdropColor);
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX + 7), markerForColor(1));
        }
    }

    // Deferred (8x16 sprites out of first-pass scope):
    //   SUBCASE("8x16 sprite tile index bit 0 selects the pattern table half")
    //   SUBCASE("8x16 sprite spans two tiles across rows 0-7 and 8-15")
    //   SUBCASE("8x16: vertical flip also swaps the top and bottom tile")
}

TEST_CASE("Sprite/background priority muxing")
{
    // Priority mux per dot (BG and SP are 2-bit; 0 = transparent; attr bit 5: 0 = sprite in front):
    //   BG=0, SP=0           -> backdrop
    //   BG=0, SP!=0          -> sprite
    //   BG!=0, SP=0          -> background
    //   BG!=0, SP!=0, front  -> sprite
    //   BG!=0, SP!=0, behind -> background
    // Background pixels read back as bgMarkerForColor(c) (0x1x); sprite pixels as markerForColor(c)
    // (0x2x), so a composed pixel reveals which layer won. Sprite is in range on display lines y+1..y+8.
    constexpr uint8 y       = 100;
    constexpr uint8 spriteX = 16; // clear of the left-column clip region

    SpriteChrCartridge chr;
    CiRam              ciram;
    PpuBus             ppuBus(ciram);
    InterruptLines     interruptLines;
    Ppu                ppu(ppuBus, interruptLines);
    ppuBus.attachCartridge(chr);

    SUBCASE("transparent background, transparent sprite -> backdrop") {
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX); // sprite tile left transparent (planeLo all 0)
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), kGlobalBackdropColor);
    }

    SUBCASE("transparent background, opaque sprite -> sprite pixel") {
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(1));
    }

    SUBCASE("opaque background, transparent sprite -> background pixel") {
        setOpaqueBackground(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX); // transparent sprite
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), bgMarkerForColor(1));
    }

    SUBCASE("opaque background, opaque front-priority sprite -> sprite pixel") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX); // attr bit 5 = 0 -> in front
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(1));
    }

    SUBCASE("opaque background, opaque behind-priority sprite -> background pixel") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x20, spriteX); // attr bit 5 = 1 -> behind background
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), bgMarkerForColor(1));
    }

    SUBCASE("sprite palette select uses attribute bits 0-1 (sprite palette 1 -> $3F15)") {
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x01, spriteX); // palette select 1
        CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), 0x25);
    }

    SUBCASE("sprite-vs-sprite priority") {
        // Tile opaque (color 1) in the left four columns, transparent in the right four.
        chr.planeLo.fill(0xF0);

        SUBCASE("lower OAM index wins where two opaque sprites overlap") {
            chr.planeLo.fill(0xFF); // both fully opaque
            setupSpriteScene(ppu);
            writeSprite(ppu, 0, y, 0, 0x00, spriteX); // sprite 0: palette 0 -> 0x21
            writeSprite(ppu, 1, y, 0, 0x01, spriteX); // sprite 1: palette 1 -> 0x25
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(1)); // sprite 0 wins
        }

        SUBCASE("a higher-index sprite shows through a lower-index transparent pixel") {
            setupSpriteScene(ppu);
            writeSprite(ppu, 0, y, 0, 0x00, spriteX); // opaque at spriteX..+3, transparent +4..+7
            writeSprite(ppu, 1, y, 0, 0x01, spriteX + 4); // opaque at spriteX+4..+7 (palette 1)

            // At spriteX sprite 0 is opaque and wins (anchor); at spriteX+4 sprite 0 is transparent,
            // so the higher-index sprite 1 must show through.
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX), markerForColor(1)); // 0x21
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, spriteX + 4), 0x25);
        }
    }

    /*
    SUBCASE("left-column clipping") {
        setOpaqueSpriteTile(chr);

        SUBCASE("sprites hidden in leftmost 8 pixels when sprite column mask is off") {
            setupSpriteScene(ppu); // mask 0x18: sprite left-column bit clear
            writeSprite(ppu, 0, y, 0, 0x00, 0);
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, 0), kGlobalBackdropColor); // clipped
        }

        SUBCASE("sprites shown in leftmost 8 pixels when sprite column mask is on") {
            setupSpriteScene(ppu);
            ppu.onCpuWrite(kPpuMask, 0x1C); // add sprite left-column enable (bit 2)
            writeSprite(ppu, 0, y, 0, 0x00, 0);
            CHECK_EQ(renderScanlinePixel(ppu, y + 2, 0), markerForColor(1));
        }
    }
    */
}

TEST_CASE("Sprite 0 hit")
{
    // Sprite 0 hit latches when sprite 0's opaque pixel coincides with an opaque background pixel,
    // with both layers rendering enabled. The scene puts an opaque background everywhere and an
    // opaque sprite 0; subcases knock out one precondition at a time. Sprite is in range y+1..y+8.
    constexpr uint8 y       = 100;
    constexpr uint8 spriteX = 16;

    SpriteChrCartridge chr;
    CiRam              ciram;
    PpuBus             ppuBus(ciram);
    InterruptLines     interruptLines;
    Ppu                ppu(ppuBus, interruptLines);
    ppuBus.attachCartridge(chr);

    SUBCASE("set when sprite 0 opaque pixel overlaps an opaque background pixel") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y)); // before the sprite's rows
        CHECK(renderAndReadSpriteZeroHit(ppu, y + 2)); // overlap line -> hit
    }

    SUBCASE("set regardless of sprite 0 priority (behind background)") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x20, spriteX); // behind-priority still triggers the hit

        CHECK(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("not set when sprite 0 pixel is transparent") {
        setOpaqueBackground(chr);
        setupSpriteScene(ppu); // sprite tile left transparent
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("not set when background pixel is transparent") {
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu); // background tile left transparent
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("not set when sprite rendering is disabled") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        ppu.onCpuWrite(kPpuMask, 0x08); // background only
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("not set when background rendering is disabled") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        ppu.onCpuWrite(kPpuMask, 0x10); // sprites only
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("not set when the overlap occurs at x = 255") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, 255); // only column 255 is on-screen; no hit there

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("a hit from a later overlapping sprite does not suppress the sprite 0 hit") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX); // sprite 0 overlaps
        writeSprite(ppu, 1, y, 0, 0x00, spriteX); // sprite 1 overlaps the same pixel

        CHECK(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("hit is tied to primary OAM index 0, not internal-OAM slot 0") {
        // Sprite 0 is parked off-screen, so it is out of range and never copied. The first
        // in-range sprite is primary index 1, which evaluation copies into internal-OAM slot 0.
        // A naive "slot 0 triggers the hit" implementation would fire here; hardware keys the hit
        // to primary index 0 specifically, so it must stay clear. Guards against an optimization
        // that conflates the first output unit with primary sprite 0.
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        // sprite 0 left parked at Y=$FF; sprite 1 is the first in-range sprite -> internal slot 0.
        writeSprite(ppu, 1, y, 0, 0x00, spriteX);

        CHECK_FALSE(renderAndReadSpriteZeroHit(ppu, y + 2));
    }

    SUBCASE("flag is cleared on the pre-render scanline") {
        setOpaqueBackground(chr);
        setOpaqueSpriteTile(chr);
        setupSpriteScene(ppu);
        writeSprite(ppu, 0, y, 0, 0x00, spriteX);

        CHECK(renderAndReadSpriteZeroHit(ppu, y + 2)); // latched mid-frame
        ppu.executeUntil(Ppu::kFramePreRenderStart * Ppu::kFrameScanlineWidth + 2); // into the pre-render scanline (261)
        CHECK_FALSE(ppu.ppuStatus().spriteZeroHit()); // cleared at pre-render
    }

    // Deferred (left-column sprite clipping not yet implemented; see muxing test):
    //   SUBCASE("not set when the overlap is in x 0-7 and a left-column mask is off")
}
}
