#define SDL_MAIN_HANDLED true

#include "NESEmuGUI/nesemu_sdl_app.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdexcept>

void NESEmu::NESEmuApp::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error("Failed to initialize SDL");
    }

    if (!SDL_CreateWindowAndRenderer(
        "NESEmuGUI",
        256,
        240,
        0,
        &mWindow,
        &mRenderer)) {
        throw std::runtime_error("Failed to create window/renderer");
    };
}

void NESEmu::NESEmuApp::run()
{
    SDL_Event e;
    bool      quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
        SDL_RenderClear(mRenderer);
        SDL_RenderPresent(mRenderer);
        SDL_Delay(1);
    }
}

void NESEmu::NESEmuApp::shutdown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
