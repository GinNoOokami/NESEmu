#include "NESEmuGUI/nesemu_sdl_app.hpp"

int main()
{
    NESEmu::NESEmuApp app;
    app.initialize();
    app.run();
    app.shutdown();

    return 0;
}
