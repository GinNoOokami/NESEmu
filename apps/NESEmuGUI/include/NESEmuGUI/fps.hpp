#ifndef NESEMU_FPS_HPP
#define NESEMU_FPS_HPP

#include <chrono>

namespace NESEmu {
class Fps {
public:
    void initialize();
    void update();

    [[nodiscard]] float getFps() const { return m_fps; }

private:
    using Clock   = std::chrono::steady_clock;
    using Seconds = std::chrono::duration<double>;

    Clock::time_point m_lastUpdateTime{};
    double            m_elapsedSeconds{};
    double            m_framesSinceLastSecond{};
    double            m_accumulatedTime{};
    float             m_fps{};
};
}

#endif //NESEMU_FPS_HPP
