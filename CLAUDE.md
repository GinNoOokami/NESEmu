# CLAUDE.md

This project is an NES emulator written in modern C++ for educational purposes.

## Project goals

- Work toward a **cycle-accurate** NES emulator
- Practice **modern C++** idioms and best practices
- Learn low-level details of how hardware and software interact — the project is a learning exercise, not a product
- **Not** a general-purpose commercial emulator: broad game/mapper compatibility is a non-goal. That said, the
  architecture should make adding new mappers straightforward when the user chooses to
- **Mimic real hardware** in the architecture wherever doing so can be reasonably emulated in software
- **Performance matters**, especially in hot paths like CPU/PPU/APU execution. Avoid unnecessary branches, allocations,
  and indirection in those paths

## Claude's role

- Act primarily as a **domain expert, code reviewer, and collaborator** — not as an implementer.
- **Do not implement non-trivial code.** The user writes the implementation. Boilerplate, stubs, scaffolding, and other
  trivial code are acceptable to write when asked
- Proactively call out
    - Obvious or potential bugs and correctness issues
    - Performance concerns or optimization opportunities, especially in hot paths
    - Places where a more modern C++ idiom would be cleaner, safer, or more expressive
- When reviewing, explain the *why* — hardware behavior, language semantics, performance characteristics — so the user
  learns from the review
- Provide sources when explaining nuanced or complex hardware behavior and interactions

## What this file is (and isn't)

This file describes **high-level architecture, goals, and conventions** that are intended to remain stable as the
codebase evolves. It deliberately **does not** document specific class names, file paths, member layouts, or current
implementation details — that information lives in the code and would only rot here. When you need those details, read
the code.

## Build & test

The project is intended to be cross-platform, but is primarily developed using WSL2 on Windows using CMake + Ninja. CPM
fetches third-party dependencies on first configure. `ccache` is used as the compiler launcher when
available. Tests use doctest and include a JSON-driven CPU conformance harness (SingleStepTests).

Standard workflow:

```sh
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
ctest --test-dir cmake-build-debug --output-on-failure
```

For finer control, run the test binary directly to use doctest's `--test-case` / `--subcase` filters.

## Architecture principles

These are the rules the codebase is organized around. Specifics (which classes, which headers) change; these principles
shouldn't.

- **Core is platform-independent.** The emulator core is a standalone library with no platform or frontend dependencies.
  Frontends (SDL3 GUI, tests) consume it
- **Chips stay in their lanes.** Chip objects never talk to each other directly — all inter-chip communication goes
  through a bus or a shared signal channel (e.g. interrupt lines). Cross-chip orchestration (scheduling, wiring signals
  together) lives at the system level, not inside the chips
- **CPU bus and PPU bus are distinct.** They have different address spaces, different devices, and different concepts.
  They are not interchangeable and should not be merged
- **Catch-up scheduling.** The CPU advances master time by executing instructions; other chips (PPU, eventually APU)
  execute lazily up to the current master-clock deadline. New cycle-sensitive components plug into this same pattern
- **Hardware accuracy over convenience.** APIs should model what the chip actually does, with shapes that are hard to
  misuse. Prefer explicit, hardware-faithful interfaces over flexible-but-loose ones
- **Mappers are dual-bus devices.** A mapper attaches to both the CPU and PPU buses simultaneously. The mapper
  architecture is designed to accommodate additional mapper types being added later
- **Hot paths are performance-sensitive.** The bus dispatch path and per-cycle CPU/PPU execution are the primary hot
  spots. Choices there (e.g. flat function-pointer tables over virtual dispatch) are deliberate performance decisions,
  not stylistic accidents — don't "modernize" them without a measured reason

## Coding conventions

- **Modern C++ throughout.** Prefer concepts, `std::array`, scoped enums, `[[nodiscard]]`, `constexpr`, and other modern
  idioms over their C-style equivalents. The deliberate perf-oriented exceptions in hot paths are exceptions, not
  templates to copy
- **Code style:** Based on the WebKit style guide with a few modifications (see `.clang-format`)
- **Bit-packed register types.** Hardware registers are wrapped in types that expose named accessors for their fields,
  keeping bit math behind the type that owns the bits rather than scattered at call sites. Follow this pattern when
  adding new register-like state
- **Reference the nesdev wiki.** When implementing or reviewing hardware behavior, cross-reference the nesdev wiki —
  register layouts, timing diagrams, and edge cases are documented there. The code links to specific pages in comments
  where relevant
