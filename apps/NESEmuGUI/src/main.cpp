#define SDL_MAIN_HANDLED true
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "NESEmuGUI",
        256,
        240,
        0
        );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    SDL_Event e;
    bool      quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
