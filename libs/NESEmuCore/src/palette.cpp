#include "NESEmuCore/palette.hpp"

using namespace NESEmu;

constexpr Palette Palette::makeDefaultPalette()
{
    Palette palette{};

    for (int i = 0; i < kPaletteSize; ++i) {
        const int dataIndex = i * 3;

        const auto r = static_cast<uint32>(kDefaultPaletteData[dataIndex]);
        const auto g = static_cast<uint32>(kDefaultPaletteData[dataIndex + 1]);
        const auto b = static_cast<uint32>(kDefaultPaletteData[dataIndex + 2]);

        palette.m_colors[i] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
    }
    return palette;
}

constexpr Palette Palette::defaultPalette = makeDefaultPalette();

