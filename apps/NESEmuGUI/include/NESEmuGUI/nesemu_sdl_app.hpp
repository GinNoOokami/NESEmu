#ifndef NESEMU_SDL_APP_HPP
#define NESEMU_SDL_APP_HPP

#include "app.hpp"

#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace NESEmu {
class System;

class NESEmuSdlApp : public App {
public:
    NESEmuSdlApp();
    ~NESEmuSdlApp() override;

    void initialize() override;
    void run() override;
    void shutdown() override;

private:
    void initSdl();
    void handleSdlEvents();
    void shutdownSdl() const;

    void handleMainLoop();
    void update();
    void render();

    SDL_Window*   mWindow{};
    SDL_Renderer* mRenderer{};

    std::unique_ptr<System> mSystem{};

    float    mElapsedTime{};
    float    mNextFrame{};
    float    mLastFrame{};
    float    mIdleTime{};
    float    mAvgIdleTime{};
    uint32_t mTotalFrames{};

    bool mInitialized{};
    bool mRunning{};
};
}

#endif //NESEMU_SDL_APP_HPP
