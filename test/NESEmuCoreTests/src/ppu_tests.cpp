#include "NESEmuCore/ppu.hpp"

#include <doctest.h>

using namespace NESEmu;

TEST_SUITE("PPU Tests") {
TEST_CASE("PPUCTRL")
{
    constexpr uint16 ppuCtrl = 0x2000;
    Ppu              ppu;

    SUBCASE("write") {
        SUBCASE("baseNametableAddress returns expected") {
            SUBCASE("$2000") {
                ppu.write(ppuCtrl, 0xF0);
                CHECK((ppu.ppuCtrl().baseNametableAddress() == 0));
            }
            SUBCASE("$2400") {
                ppu.write(ppuCtrl, 0xF1);
                CHECK((ppu.ppuCtrl().baseNametableAddress() == 1));
            }

            SUBCASE("$2800") {
                ppu.write(ppuCtrl, 0xF2);
                CHECK((ppu.ppuCtrl().baseNametableAddress() == 2));
            }
            SUBCASE("$2C00") {
                ppu.write(ppuCtrl, 0xF3);
                CHECK((ppu.ppuCtrl().baseNametableAddress() == 3));
            }
        }

        SUBCASE("ramAddressIncrement returns expected") {
            SUBCASE("across") {
                ppu.write(ppuCtrl, 0xFB);
                CHECK_FALSE(ppu.ppuCtrl().ramAddressIncrement());
            }
            SUBCASE("down") {
                ppu.write(ppuCtrl, 0xF4);
                CHECK(ppu.ppuCtrl().ramAddressIncrement());
            }
        }

        SUBCASE("spritePatternTableAddress returns expected") {
            SUBCASE("$0000") {
                ppu.write(ppuCtrl, 0xF7);
                CHECK_FALSE(ppu.ppuCtrl().spritePatternTableAddress());
            }
            SUBCASE("$1000") {
                ppu.write(ppuCtrl, 0xF8);
                CHECK(ppu.ppuCtrl().spritePatternTableAddress());
            }
        }

        SUBCASE("backgroundPatternTableAddress returns expected") {
            SUBCASE("$0000") {
                ppu.write(ppuCtrl, 0xEF);
                CHECK_FALSE(ppu.ppuCtrl().backgroundPatternTableAddress());
            }
            SUBCASE("$1000") {
                ppu.write(ppuCtrl, 0xFF);
                CHECK(ppu.ppuCtrl().backgroundPatternTableAddress());
            }
        }

        SUBCASE("spriteSize returns expected") {
            SUBCASE("8x8") {
                ppu.write(ppuCtrl, 0xDF);
                CHECK_FALSE(ppu.ppuCtrl().spriteSize());
            }
            SUBCASE("8x16") {
                ppu.write(ppuCtrl, 0x7F);
                CHECK(ppu.ppuCtrl().spriteSize());
            }
        }

        SUBCASE("masterSlaveSelect returns expected") {
            SUBCASE("read backdrop") {
                ppu.write(ppuCtrl, 0xAF);
                CHECK_FALSE(ppu.ppuCtrl().masterSlaveSelect());
            }
            SUBCASE("output color") {
                ppu.write(ppuCtrl, 0xFF);
                CHECK(ppu.ppuCtrl().masterSlaveSelect());
            }
        }

        SUBCASE("enableNmi returns expected") {
            SUBCASE("disabled") {
                ppu.write(ppuCtrl, 0x7F);
                CHECK_FALSE(ppu.ppuCtrl().nmiEnable());
            }
            SUBCASE("enabled") {
                ppu.write(ppuCtrl, 0xFF);
                CHECK(ppu.ppuCtrl().nmiEnable());
            }
        }
    }
    SUBCASE("reads open bus latch") {
        ppu.write(ppuCtrl, 0x55);
        CHECK((ppu.read(ppuCtrl) == 0x55));
    }
}
}
