#define SDL_MAIN_HANDLED true

#include "NESEmuGUI/nesemu_sdl_app.hpp"

#include "NESEmuCore/palette.hpp"
#include "NESEmuCore/system.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdexcept>

NESEmu::NESEmuSdlApp::NESEmuSdlApp()
    : mSystem(std::make_unique<System>()), mPalette(Palette::defaultPalette) {}

NESEmu::NESEmuSdlApp::~NESEmuSdlApp() = default;

void NESEmu::NESEmuSdlApp::initialize(NESEmuArgs args)
{
    initSdl();

    if (!args.cartridgeFilename.empty() && std::filesystem::exists(args.cartridgeFilename)) {
        loadAndPlayCartridge(args.cartridgeFilename);
    }

    mInitialized = true;
}

void NESEmu::NESEmuSdlApp::run()
{
    mRunning = true;

    while (mRunning) {
        handleSdlEvents();
        handleMainLoop();
    }
}

void NESEmu::NESEmuSdlApp::shutdown()
{
    if (mInitialized) {
        stopAndUnloadCartridge();
        shutdownSdl();

        mInitialized = false;
    }
}

void NESEmu::NESEmuSdlApp::initSdl()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error("Failed to initialize SDL");
    }

    if (!SDL_CreateWindowAndRenderer(
        "NESEmuGUI",
        kScreenDotWidth * kScreenScale,
        kScreenDotHeight * kScreenScale,
        0,
        &mWindow,
        &mRenderer)) {
        throw std::runtime_error("Failed to create window/renderer");
    }

    mTexture = SDL_CreateTexture(
        mRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        kScreenDotWidth,
        kScreenDotHeight);
    if (!mTexture) {
        throw std::runtime_error("Failed to create main texture");
    }

    // Set point filtering
    SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_NEAREST);

    // Clear to white
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    SDL_SetWindowTitle(mWindow, "NES Emulator");
}

void NESEmu::NESEmuSdlApp::handleSdlEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            mRunning = false;
        }
    }
}

void NESEmu::NESEmuSdlApp::shutdownSdl() const
{
    SDL_DestroyTexture(mTexture);
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void NESEmu::NESEmuSdlApp::loadAndPlayCartridge(const std::string& filename)
{
    mCartridge = Cartridge::createFromFile(filename);
    mSystem->startup(*mCartridge);
}

void NESEmu::NESEmuSdlApp::stopAndUnloadCartridge()
{
    if (mSystem->cartridgeLoaded()) {
        mSystem->shutdown();
    }
}

void NESEmu::NESEmuSdlApp::handleMainLoop()
{
    const float kFrameRate = static_cast<float>(System::kMasterClockFrameCycles) / static_cast<float>(mSystem->targetMasterFrameCycles());
    const float kFrameTime = 1000.f / static_cast<float>(kFrameRate);

    mElapsedTime += static_cast<float>(SDL_GetTicks()) - mElapsedTime;

    if (mElapsedTime >= mNextFrame) {
        float delta = mElapsedTime - mNextFrame;

        // Make sure we don't render faster than our framerate
        delta = delta > kFrameTime ? kFrameTime : delta;

        // Determine how much time was spent idle this frame since the end of the last emulation step
        mIdleTime += mNextFrame - mLastFrame;
        ++mTotalFrames;

        // Calculate the idle time over the last second and reset counters
        if (static_cast<float>(mTotalFrames) >= kFrameRate) {
            mAvgIdleTime = mIdleTime / static_cast<float>(mTotalFrames);
            mIdleTime    = 0;
            mTotalFrames = 0;
        }

        // Set the next target frame update time
        mNextFrame = mElapsedTime + kFrameTime - delta;

        update();
        render();

        // Calculate the last frame time after emulation step
        mLastFrame = static_cast<float>(SDL_GetTicks());
    } else if (mNextFrame - mElapsedTime > 1.f) {
        // Wait a bit if we can, so we don't hog the cpu
        SDL_Delay(1);
    }
}

void NESEmu::NESEmuSdlApp::update()
{
    if (mSystem->cartridgeLoaded()) {
        mSystem->runFrame();
    }
}

void NESEmu::NESEmuSdlApp::render()
{
    SDL_RenderClear(mRenderer);

    if (mSystem->cartridgeLoaded()) {
        updateFrameTexture();
        SDL_RenderTexture(mRenderer, mTexture, nullptr, nullptr);
    }

    SDL_RenderPresent(mRenderer);
}

void NESEmu::NESEmuSdlApp::updateFrameTexture()
{
    const auto frameBuffer = mSystem->frameBuffer();

    uint32_t* pixels;
    int       pitch;

    // TODO: Convert to shader instead of software rendering
    if (SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch)) {
        const int texturePitchPixels = pitch / static_cast<int>(sizeof(uint32_t));

        for (int y = 0; y < kScreenDotHeight; ++y) {
            for (int x = 0; x < kScreenDotWidth; ++x) {
                const PaletteIndex index = frameBuffer[y * kScreenDotWidth + x];

                pixels[y * texturePitchPixels + x] = mPalette.getColor(index);
            }
        }
        SDL_UnlockTexture(mTexture);
    }
}
