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

    SDL_Window*   m_window{};
    SDL_Renderer* m_renderer{};
    SDL_Texture*  m_texture{};

    std::unique_ptr<Cartridge> m_cartridge{};
    std::unique_ptr<System>    m_system{};
    const Palette&             m_palette;

    float    m_elapsedTime{};
    float    m_nextFrame{};
    float    m_lastFrame{};
    float    m_idleTime{};
    float    m_avgIdleTime{};
    uint32_t m_totalFrames{};

    bool m_initialized{};
    bool m_running{};
};
}

#endif //NESEMU_SDL_APP_HPP
