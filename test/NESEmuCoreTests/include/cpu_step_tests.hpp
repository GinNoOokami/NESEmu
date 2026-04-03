#ifndef NESEMU_CPU_STEP_TESTS_HPP
#define NESEMU_CPU_STEP_TESTS_HPP

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

#include "NESEmuCore/cpu_6502.hpp"

using json = nlohmann::json;

namespace NESEmu {
    struct CPUTestState {
        CPU_6502::CPUState_6502 cpu {};
        std::vector<std::tuple<uint16, uint8>> memory;
    };

    static void to_json(json& j, const CPUTestState& state) {
        j = json{
          {"pc", state.cpu.pc},
          {"s", state.cpu.sp},
          {"a", state.cpu.a},
          {"x", state.cpu.x},
          {"y", state.cpu.y},
          {"p", state.cpu.p},
          {"ram", state.memory},
        };
    }

    static void from_json(const json& j, CPUTestState& state) {
        j.at("pc").get_to(state.cpu.pc);
        j.at("s").get_to(state.cpu.sp);
        j.at("a").get_to(state.cpu.a);
        j.at("x").get_to(state.cpu.x);
        j.at("y").get_to(state.cpu.y);
        j.at("p").get_to(state.cpu.p);
        j.at("ram").get_to(state.memory);
    }

    struct CpuStepTest {
        std::string name;
        CPUTestState initial_state {};
        CPUTestState final_state {};

        static CpuStepTest from_json(uint8 opcode) {
            std::ostringstream oss;
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint32>(opcode);

            auto path = std::string(TEST_DATA_DIR) + "/" + oss.str() + ".json";
            std::ifstream f(path);
            json data = json::parse(f)[0];

            CpuStepTest cpu_step_test_state {
                data["name"],
                data.at("initial").get<CPUTestState>(),
                data.at("final").get<CPUTestState>()
            };

            return cpu_step_test_state;
        }
    };
}

#endif //NESEMU_CPU_STEP_TESTS_HPP
