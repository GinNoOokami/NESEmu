#include "NESEmuGUI/nesemu_sdl_app.hpp"

#include <memory>

int main()
{
    const std::unique_ptr<App> app = std::make_unique<NESEmu::NESEmuSdlApp>();

    app->initialize();
    app->run();
    app->shutdown();

    return 0;
}
