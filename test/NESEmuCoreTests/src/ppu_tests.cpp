#include "NESEmuCore/ppu.hpp"
#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/interrupt_lines.hpp"

#include <doctest.h>

using namespace NESEmu;

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

                CHECK_FALSE(ppu.ppuCtrl().ramAddressIncrement());
            }
            SUBCASE("down") {
                ppu.onCpuWrite(ppuCtrl, 0xF4);

                CHECK(ppu.ppuCtrl().ramAddressIncrement());
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
