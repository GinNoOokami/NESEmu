#include "NESEmuGUI/nesemu_sdl_app.hpp"
#include "NESEmuGUI/nesemu_args.hpp"

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
    NESEmu::NESEmuArgs args;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--cartridge" || std::string(argv[i]) == "-c") {
                args.cartridgeFilename = argv[++i];
            }
        }
    }

    std::cout << args.cartridgeFilename << std::endl;

    const std::unique_ptr<App<NESEmu::NESEmuArgs>> app = std::make_unique<NESEmu::NESEmuSdlApp>();

    app->initialize(args);
    app->run();
    app->shutdown();

    return 0;
}
