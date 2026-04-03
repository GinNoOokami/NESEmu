#ifndef NESEMU_CPU_STEP_TESTS_HPP
#define NESEMU_CPU_STEP_TESTS_HPP

#include <nlohmann/json.hpp>
#include <iostream>

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

        static CpuStepTest from_json() {
          using namespace nlohmann::literals;
                  auto json = R"(
{
  "name": "e8 9f f9",
  "initial": {
    "pc": 35594,
    "s": 199,
    "a": 205,
    "x": 144,
    "y": 179,
    "p": 162,
    "ram": [
      [
        35594,
        232
      ],
      [
        35595,
        159
      ],
      [
        35596,
        249
      ]
    ]
  },
  "final": {
    "pc": 35595,
    "s": 199,
    "a": 205,
    "x": 145,
    "y": 179,
    "p": 160,
    "ram": [
      [
        35594,
        232
      ],
      [
        35595,
        159
      ],
      [
        35596,
        249
      ]
    ]
  },
  "cycles": [
    [
      35594,
      232,
      "read"
    ],
    [
      35595,
      159,
      "read"
    ]
  ]
}
          )"_json;

          CpuStepTest cpu_step_test_state {
            json["name"],
            json.at("initial").get<CPUTestState>(),
            json.at("final").get<CPUTestState>()
          };

          return cpu_step_test_state;
        }
    };
}

#endif //NESEMU_CPU_STEP_TESTS_HPP
