#include "NESEmuGUI/fps.hpp"

using namespace NESEmu;

void Fps::initialize()
{
    m_lastUpdateTime = Clock::now();
}

void Fps::update()
{
    const auto    currentTime = Clock::now();
    const Seconds deltaTime   = currentTime - m_lastUpdateTime;

    m_lastUpdateTime        = currentTime;
    m_accumulatedTime       += deltaTime.count();
    m_framesSinceLastSecond += 1.0;

    // Update FPS after one second elapsed
    if (m_accumulatedTime >= 1.0) {
        m_fps = static_cast<float>(m_framesSinceLastSecond / m_accumulatedTime);

        m_accumulatedTime       = 0.0;
        m_framesSinceLastSecond = 0.0;
    }
}
