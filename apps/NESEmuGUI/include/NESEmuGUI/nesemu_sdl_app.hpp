#ifndef NESEMU_SDL_APP_HPP
#define NESEMU_SDL_APP_HPP

#include "app.hpp"
struct SDL_Window;
struct SDL_Renderer;

namespace NESEmu {

class NESEmuApp : public App {
public:
    ~NESEmuApp() override = default;

    void initialize() override;
    void run() override;
    void shutdown() override;

private:
    SDL_Window*   mWindow{};
    SDL_Renderer* mRenderer{};
};
}

#endif //NESEMU_SDL_APP_HPP
