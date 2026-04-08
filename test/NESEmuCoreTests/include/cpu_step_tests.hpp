#ifndef NESEMU_CPU_STEP_TESTS_HPP
#define NESEMU_CPU_STEP_TESTS_HPP

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

#include "NESEmuCore/bus.hpp"
#include "NESEmuCore/cpu6502.hpp"
#include "NESEmuCore/debug_format.hpp"

using json = nlohmann::json;

namespace NESEmu {
struct CPUTestState {
    Cpu6502::State                         cpu{};
    std::vector<std::tuple<uint16, uint8>> memory;
};

inline bool operator==(const CPUTestState& lhs, const CPUTestState& rhs)
{
    if (!(lhs.cpu == rhs.cpu)) {
        return false;
    }

    if (lhs.memory.size() != rhs.memory.size()) {
        return false;
    }

    auto lhsMemory = lhs.memory;
    auto rhsMemory = rhs.memory;

    std::ranges::sort(lhsMemory);
    std::ranges::sort(rhsMemory);

    return lhsMemory == rhsMemory;
}

inline bool operator!=(const CPUTestState& lhs, const CPUTestState& rhs)
{
    return !(lhs == rhs);
}

inline std::string cpu_test_state_to_string(const CPUTestState& state)
{
    std::ostringstream oss;
    oss << "{\n"
        << "  cpu: " << state.cpu << "\n"
        << "  memory:\n";

    for (const auto& [address, value] : state.memory) {
        oss << "    " << to_register(address) << " = " << to_register(value) << "\n";
    }

    oss << "}";
    return oss.str();
}

inline std::ostream& operator<<(std::ostream& os, const CPUTestState& value)
{
    os << cpu_test_state_to_string(value);
    return os;
}

static void to_json(json& j, const CPUTestState& state)
{
    j = json{
        { "pc", state.cpu.pc },
        { "s", state.cpu.sp },
        { "a", state.cpu.a },
        { "x", state.cpu.x },
        { "y", state.cpu.y },
        { "p", state.cpu.p },
        { "ram", state.memory },
    };
}

static void from_json(const json& j, CPUTestState& state)
{
    j.at("pc").get_to(state.cpu.pc);
    j.at("s").get_to(state.cpu.sp);
    j.at("a").get_to(state.cpu.a);
    j.at("x").get_to(state.cpu.x);
    j.at("y").get_to(state.cpu.y);
    j.at("p").get_to(state.cpu.p);
    j.at("ram").get_to(state.memory);
}

struct CpuStepTest {
    std::string                                         key;
    std::string                                         name;
    CPUTestState                                        initial_state{};
    CPUTestState                                        final_state{};
    std::vector<std::tuple<uint16, uint8, std::string>> cycles{};

    static void from_json(uint8 opcode, std::vector<CpuStepTest>& tests)
    {
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint32>(opcode);

        auto          path = std::string(TEST_DATA_DIR) + "/" + oss.str() + ".json";
        std::ifstream f(path);
        json          data = json::parse(f);

        tests.clear();

        for (const auto& [key, value] : data.items()) {
            CpuStepTest cpu_step_test_state{
                key,
                value["name"],
                value.at("initial").get<CPUTestState>(),
                value.at("final").get<CPUTestState>(),
                value.at("cycles")
            };

            tests.push_back(cpu_step_test_state);
        }
    }

    void initializeCpu(Cpu6502& cpu, Bus& bus) const
    {
        cpu.state(initial_state.cpu);
        for (auto [address, value] : initial_state.memory) {
            bus.write(address, value);
        }
    }

    [[nodiscard]] CPUTestState currentState(const Cpu6502& cpu, Bus& bus)
    {
        CPUTestState current;
        current.cpu = cpu.state();
        for (auto [address, value] : final_state.memory) {
            current.memory.emplace_back(address, bus.read(address));
        }
        return current;
    }
};
}

#endif //NESEMU_CPU_STEP_TESTS_HPP
