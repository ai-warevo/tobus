# CLAUDE.md

Guidance for working on `aw::tobus`. Read `README.md` for the user-facing API; this file is about *how to change the repo*.

## Identity & philosophy

`aw::tobus` is a transport-agnostic **event bus** for C++23 (`aw::tobus`). Modules publish events to `IEventBus` and subscribe independently — the publisher doesn't know who (or how many) subscribers exist. The transport layer (in-process, Unix socket, TCP, HTTP, WebSocket, Redis, ZeroMQ, RabbitMQ, Kafka) is selected at configuration time.

Dependencies: only [`aw::tpl`](https://github.com/ai-warevo/tpl) (coroutines) and the C++ standard library.

When designing a feature:
- Keep the transport-agnostic principle: `IEventBus` is the single interface; every transport implements it.
- New transport stubs are OFF by default (`DTOBUS_ENABLE_*`); they compile only when explicitly enabled.
- Keep the public surface small: `EventBusFactory::create()`, `IEventBus`, `EventHandler`, `Subscription`, `EventBusConfig`.
- A new transport or config option needs a README mention and an example (`examples/NN_*.cpp`).

## Build & test (fast local loop)

```sh
cmake -B build/dev -DCMAKE_BUILD_TYPE=Debug
cmake --build build/dev --parallel
ctest --test-dir build/dev --output-on-failure --parallel 4
```

- `make build` / `make test` / `make lint` / `make format` / `make clean` (Makefile).
- `clang-tidy -p build <file>` (compile_commands.json is auto-generated).
- **Leave to CI:** sanitizers (ASan/UBSan/TSan), coverage (gcovr), multi-toolchain (clang-18 libc++/libstdc++, gcc-15), consumer integration (add_subdirectory / FetchContent / find_package).

## CI/CD

GitHub Actions owns all CI and releases, reusing `ai-warevo/.github` composite actions and reusable workflows wherever they fit this project.

### Workflows

- **pr-fast.yml** (5-10 min): calls `reusable-pr-fast.yml` with fast checks — clang-format, clang-tidy, and Debug build+test for clang18-libcxx and gcc15.
- **ci.yml** (20-25 min): full matrix builds (Linux clang18-libcxx/gcc15 × Debug/Release, Windows MSVC × Debug/Release) and default-options matrix (no transport stubs enabled). All jobs use catalog composite actions (`setup-toolchain`, `cmake-build`, `ctest-run`).
- **test.yml** (30-40 min): lint (clang-format/tidy on all sources), sanitizers (ASan+UBSan Debug, TSan Release × 30 iterations), stress tests (concurrency × 30), and coverage (gcovr → JSON → parser → badge → GitHub Pages deployment).
- **consumers.yml** (25-30 min): integration tests for add_subdirectory, find_package, and FetchContent on Linux (clang18-libcxx, gcc15) and Windows MSVC.
- **release.yml**: calls `reusable-release.yml` for version resolution (from CHANGELOG.md categories), changelog promotion, tag creation, checksum generation/verification, and GitHub Release publication. No build artifacts or Docker images because this is a header+static library project.

### When adding or changing workflows

- Keep project-specific test and build logic (multi-transport matrix, examples, consumer integration) in repository workflows.
- Use catalog composite actions (`@main`) for standard CMake build/test/lint steps on all platforms (Linux, macOS, Windows).
- Document workflow changes in README.md CI/CD section and add a CHANGELOG.md entry under `## [Unreleased]`.

## Changelog & releases

`CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com) and SemVer.

- **Every user-visible change adds a bullet under `## [Unreleased]` in the same commit** (public API, behaviour, build options, CI/tooling users see). Skip pure refactors and tests with no visible effect.
- **Categories** (omit empty ones, spell exactly): `Breaking`, `Added`, `Changed`, `Deprecated`, `Removed`, `Fixed`, `Security`.
- Version bump: `Breaking`/`Removed` → major, `Added`/`Deprecated` → minor, `Fixed`/`Changed`/`Security` → patch.

Release helpers:

```sh
scripts/release.sh [--dry-run] [--push]
./scripts/release.ps1 [-DryRun] [-Push]
make release-dry-run
make release [PUSH=1]
```

## Architecture & layout rules

Namespaces map to directories:

| C++ | Path |
| --- | --- |
| `aw::tobus` | `include/aw/tobus/`, `src/` |
| `aw::tobus::transports` | `.../transports/` |
| `*::detail` | `src/detail/` |

- **Granularity:** one logical concept per header. File names are `snake_case`. `tobus.hpp` is the umbrella header.
- **Transport stubs:** each transport has a public header in `include/aw/tobus/transports/` and a private source in `src/transports/`. They are compiled only when `DTOBUS_ENABLE_<X>` is ON.
- **`.hpp`/`.cpp` separation:** non-template code is declared in the header and defined in the mirrored path under `src/`. Templates stay in headers.
- Headers start with `#pragma once`; a `.cpp` includes its own header first. Include groups: own header, then `<aw/tobus/...>`, then `<std>` headers.

## Documentation & comments

- **Every public class, function, concept and exception type gets a Doxygen `/** ... */` block.** Brief docs on `detail/` types encouraged.
- Inline `//` comments: minimal — only for non-obvious logic, subtle reasoning, or the *why*. Prefer better names over comments.
- **C++ comment syntax:** Prefer C-style block comments `/* ... */` for multi-line or longer single-line explanations. Reserve `//` for rare, ultra-short inline notes or tool directives (`// NOLINT`). Never use `//` to narrate what code does (the reader knows C++).
- Keep README tables/examples in sync when public behaviour changes.

## C++23 conventions & style

- Idiomatic modern C++23: concepts, `requires`, `if constexpr`, coroutines, RAII.
- `[[nodiscard]]` on factories and pure getters; `noexcept` where truly non-throwing; `explicit` on single-arg constructors.
- Naming: types `PascalCase`; functions/variables/files `snake_case`; private data members `trailing_underscore_`.
- Formatting (`.clang-format`, `.editorconfig`): 4 spaces, 120 cols, attached braces, `Type* p` / `Type& r`, no namespace indentation, closing comments `} // namespace aw::tobus`.

### Dev tooling

- `.editorconfig` – indentation/charset/whitespace.
- `.clang-format` – matches style (clang-format ≥ 15). Format only files you touch.
- `.clang-tidy` – full check set minus exclusions. Run with `clang-tidy -p build <file>`.
- Top-level CMake sets `CMAKE_EXPORT_COMPILE_COMMANDS=ON` for clangd/clang-tidy.
