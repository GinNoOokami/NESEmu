#ifndef NESEMU_SDL_APP_HPP
#define NESEMU_SDL_APP_HPP

#include "app.hpp"
#include "NESEmuCore/emu_types.hpp"
#include "nesemu_args.hpp"

#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace NESEmu {
class Palette;
class Cartridge;
class System;

class NESEmuSdlApp : public App<NESEmuArgs> {
    static constexpr uint8 kScreenScale = 4;
    
public:
    NESEmuSdlApp();
    ~NESEmuSdlApp() override;

    void initialize(NESEmuArgs args) override;
    void run() override;
    void shutdown() override;

private:
    void initSdl();
    void handleSdlEvents();
    void shutdownSdl() const;

    void loadAndPlayCartridge(const std::string& filename);
    void stopAndUnloadCartridge();

    void handleMainLoop();
    void update();
    void render();
    void updateFrameTexture();

    SDL_Window*   mWindow{};
    SDL_Renderer* mRenderer{};
    SDL_Texture*  mTexture{};

    std::unique_ptr<Cartridge> mCartridge{};
    std::unique_ptr<System>    mSystem{};
    const Palette&             mPalette;

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
