#define SDL_MAIN_HANDLED true
#include <SDL.h>

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/cpu6502.hpp"
#include "NESEmuCore/memory.hpp"

using namespace NESEmu;

int main()
{
    InternalRam   memory;
    CpuBus    bus(memory);
    const Cpu6502 cpu(bus);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "NESEmuGUI",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        256,
        240,
        0
        );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event e;
    bool      quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(0);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
