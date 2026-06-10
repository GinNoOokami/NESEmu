#define SDL_MAIN_HANDLED true

#include "NESEmuGUI/nesemu_sdl_app.hpp"

#include "NESEmuCore/cartridge.hpp"
#include "NESEmuCore/controller.hpp"
#include "NESEmuCore/palette.hpp"
#include "NESEmuCore/system.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include <stdexcept>

NESEmu::NESEmuSdlApp::NESEmuSdlApp()
    : m_system(std::make_unique<System>()),
      m_controller(std::make_unique<Controller>()),
      m_palette(Palette::defaultPalette) {}

NESEmu::NESEmuSdlApp::~NESEmuSdlApp() = default;

void NESEmu::NESEmuSdlApp::initialize(NESEmuArgs args)
{
    initSdl();

    if (!args.cartridgeFilename.empty() && std::filesystem::exists(args.cartridgeFilename)) {
        loadAndPlayCartridge(args.cartridgeFilename);
    }

    m_fps.initialize();

    m_initialized = true;
}

void NESEmu::NESEmuSdlApp::run()
{
    m_running = true;

    while (m_running) {
        handleSdlEvents();
        handleMainLoop();
    }
}

void NESEmu::NESEmuSdlApp::shutdown()
{
    if (m_initialized) {
        stopAndUnloadCartridge();
        shutdownSdl();

        m_initialized = false;
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
        &m_window,
        &m_renderer)) {
        throw std::runtime_error("Failed to create window/renderer");
    }

    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        kScreenDotWidth,
        kScreenDotHeight);
    if (!m_texture) {
        throw std::runtime_error("Failed to create main texture");
    }

    // Set point filtering
    SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST);

    // Clear to white
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

    SDL_SetWindowTitle(m_window, "NES Emulator");
}

void NESEmu::NESEmuSdlApp::handleSdlEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            m_running = false;
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            handleSdlKeyPressedEvent(e.key);
        } else if (e.type == SDL_EVENT_KEY_UP) {
            handleSdlKeyReleasedEvent(e.key);
        }
    }
}

void NESEmu::NESEmuSdlApp::handleSdlKeyPressedEvent(const SDL_KeyboardEvent& key)
{
    switch (key.key) {
        case SDLK_A:
            m_controller->simulatePress(InputButtons::A);
            break;
        case SDLK_S:
            m_controller->simulatePress(InputButtons::B);
            break;
        case SDLK_RSHIFT:
            m_controller->simulatePress(InputButtons::Select);
            break;
        case SDLK_RETURN:
            m_controller->simulatePress(InputButtons::Start);
            break;
        case SDLK_UP:
            m_controller->simulatePress(InputButtons::Up);
            break;
        case SDLK_DOWN:
            m_controller->simulatePress(InputButtons::Down);
            break;
        case SDLK_LEFT:
            m_controller->simulatePress(InputButtons::Left);
            break;
        case SDLK_RIGHT:
            m_controller->simulatePress(InputButtons::Right);
            break;
        default:
            // Do nothing
            break;
    }
}

void NESEmu::NESEmuSdlApp::handleSdlKeyReleasedEvent(const SDL_KeyboardEvent& key)
{
    switch (key.key) {
        case SDLK_A:
            m_controller->simulateRelease(InputButtons::A);
            break;
        case SDLK_S:
            m_controller->simulateRelease(InputButtons::B);
            break;
        case SDLK_RSHIFT:
            m_controller->simulateRelease(InputButtons::Select);
            break;
        case SDLK_RETURN:
            m_controller->simulateRelease(InputButtons::Start);
            break;
        case SDLK_UP:
            m_controller->simulateRelease(InputButtons::Up);
            break;
        case SDLK_DOWN:
            m_controller->simulateRelease(InputButtons::Down);
            break;
        case SDLK_LEFT:
            m_controller->simulateRelease(InputButtons::Left);
            break;
        case SDLK_RIGHT:
            m_controller->simulateRelease(InputButtons::Right);
            break;
        default:
            // Do nothing
            break;
    }
}

void NESEmu::NESEmuSdlApp::shutdownSdl() const
{
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void NESEmu::NESEmuSdlApp::loadAndPlayCartridge(const std::string& filename)
{
    m_cartridge = Cartridge::createFromFile(filename);
    m_system->attachController(InputPort::Joypad1, m_controller.get());
    m_system->startup(*m_cartridge);
}

void NESEmu::NESEmuSdlApp::stopAndUnloadCartridge()
{
    if (m_system->cartridgeLoaded()) {
        m_system->shutdown();
    }
}

void NESEmu::NESEmuSdlApp::handleMainLoop()
{
    const float kFrameRate = static_cast<float>(System::kMasterClockSpeedHz) / static_cast<float>(m_system->targetMasterFrameCycles());
    const float kFrameTime = 1000.f / static_cast<float>(kFrameRate);

    m_elapsedTime = static_cast<float>(SDL_GetTicks());

    if (m_elapsedTime >= m_nextFrame) {
        float delta = m_elapsedTime - m_nextFrame;

        // Make sure we don't render faster than our framerate
        delta = delta > kFrameTime ? kFrameTime : delta;

        // Determine how much time was spent idle this frame since the end of the last emulation step
        m_idleTime += m_nextFrame - m_lastFrame;
        ++m_totalFrames;

        // Calculate the idle time over the last second and reset counters
        if (static_cast<float>(m_totalFrames) >= kFrameRate) {
            m_avgIdleTime = m_idleTime / static_cast<float>(m_totalFrames);
            m_idleTime    = 0;
            m_totalFrames = 0;
        }

        // Set the next target frame update time
        m_nextFrame = m_elapsedTime + kFrameTime - delta;

        update();
        render();

        // Calculate the last frame time after emulation step
        m_lastFrame = static_cast<float>(SDL_GetTicks());

        m_fps.update();

        const auto title = std::format("NES Emulator - {:.2f} fps", m_fps.getFps());
        SDL_SetWindowTitle(m_window, title.c_str());
    } else if (m_nextFrame - m_elapsedTime > 1.f) {
        // Wait a bit if we can, so we don't hog the cpu
        SDL_Delay(1);
    }
}

void NESEmu::NESEmuSdlApp::update()
{
    if (m_system->cartridgeLoaded()) {
        m_system->runFrame();
    }
}

void NESEmu::NESEmuSdlApp::render()
{
    SDL_RenderClear(m_renderer);

    if (m_system->cartridgeLoaded()) {
        updateFrameTexture();
        SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
    }

    SDL_RenderPresent(m_renderer);
}

void NESEmu::NESEmuSdlApp::updateFrameTexture()
{
    const auto frameBuffer = m_system->frameBuffer();

    uint32_t* pixels;
    int       pitch;

    // TODO: Convert to shader instead of software rendering
    if (SDL_LockTexture(m_texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch)) {
        const int texturePitchPixels = pitch / static_cast<int>(sizeof(uint32_t));

        for (int y = 0; y < kScreenDotHeight; ++y) {
            for (int x = 0; x < kScreenDotWidth; ++x) {
                const PaletteIndex index = frameBuffer[y * kScreenDotWidth + x];

                pixels[y * texturePitchPixels + x] = m_palette.getColor(index);
            }
        }
        SDL_UnlockTexture(m_texture);
    }
}
