# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

<!--
Maintainer notes (see CLAUDE.md, "Changelog & releases"):
- Add every user-visible change under `## [Unreleased]` in the same commit that makes it.
- Categories: Breaking, Added, Changed, Deprecated, Removed, Fixed, Security. Omit empty ones.
- scripts/release.sh / scripts/release.ps1 derive the next version from those categories.
-->

## [Unreleased]

### Added

- Comprehensive `FramedBus` test coverage for edge cases: origin filtering, deserialization errors, type collisions, and transport error handling in `test_framed_bus_coverage.cpp`
- Comprehensive error exception test coverage in `test_errors_coverage.cpp`: all `TransportKind` values, exception message formatting, and inheritance hierarchy

## [0.5.6] - 2026-11-22

### Changed

- use reusable-coverage-badge workflow
- Merge branch 'main' of github.com:ai-warevo/tobus

### Fixed

- enable coverage badge updates for scheduled/manual runs

## [0.5.5] - 2026-11-22

### Changed

- Merge branch 'main' of github.com:ai-warevo/tobus

### Fixed

- mark unused EventTraits::name as [[maybe_unused]]

## [0.5.4] - 2026-11-22

### Changed

- Merge branch 'main' of github.com:ai-warevo/tobus
- fix clang-tidy lint errors in chat example

## [0.5.3] - 2026-11-22

### Changed

- use shared template_cpp CMake helpers for target and example setup
- clang-tidy
- clang-tidy
- fix
- cmake
- Merge branch 'main' of github.com:ai-warevo/tobus
- finalize canonical clang-tidy configuration with all exclusions
- merge project-specific clang-tidy exclusions into canonical config
- synchronize clang-tidy config from template-cpp

### Fixed

- configure shared warning flags after project

## [0.5.2] - 2026-11-21

### Changed

- lint
- ci

## [0.5.1] - 2026-11-21

### Changed

- Merge pull request #4 from ai-warevo/loktionov129-patch-2
- Update README.md
- Merge branch 'main' of github.com:ai-warevo/tobus
- fixiik

## [0.5.0] - 2026-11-21

### Added

- add curl testing guide and event traits

### Changed

- remove consumers.yml workflow
- remove redundant consumer integration tests
- restore consumer integration tests after cmake restructure
- fix cmake
- remove local cmake/ and scripts/, unify with tpl structure
- ci
- ci
- Merge branch 'main' of github.com:ai-warevo/tobus
- Merge remote-tracking branch 'origin/main' into fix/validate-project-workflows
- Merge pull request #2 from ai-vibeqodez/fix/validate-project-workflows
- improve Windows example discovery with recursive find
- explicitly enable examples on Windows MSVC jobs
- add diagnostics for Windows example build path
- fix Windows MSVC multi-config build paths
- disable broker transports on Windows MSVC
- remove test steps from Windows MSVC jobs entirely
- use correct YAML if condition for Windows test steps
- skip test run step on Windows MSVC
- update CHANGELOG for Windows test disabling
- disable test building on Windows MSVC to avoid Catch2 linker issues
- update CHANGELOG to mention test code fix for Catch2 MSVC
- add diagnostic output for Catch2 cache settings
- pass Catch2 config options as CMake arguments for Windows MSVC jobs
- use shared actions for Windows MSVC jobs
- add CI/CD section to AGENTS.md
- Merge pull request #1 from ai-vibeqodez/feat/integrate-reusable-ci
- integrate shared reusable workflows
- c++23
- http_relay output
- ru -> en

### Fixed

<<<<<<< HEAD
- Use MSVC-compatible warning flags for examples and tests instead of passing GCC/Clang `-Wextra` to `cl`.
- Resolved integer sign-conversion warnings in `bus.hpp`, `event_queue.hpp`, and `BinaryEventTraits` serialize/deserialize helpers with explicit casts for `-Wconversion -Wsign-conversion -Wimplicit-int-conversion` compliance.
- CMakeUserPresets.json inheritance path corrected from missing `dev` preset to existing `debug` preset.
- Correct consumer workflow source paths for Linux containers and format all sources exercised by clang-format CI.
- Fix CI-only transport option and example executable paths; make HTTP hex encoding warning-clean under Clang `-Werror`.
- Catch2 v3 on Windows MSVC: disable test compilation due to unresolved external symbol linker errors with StringMaker specializations in v3.7.1. Consumer integration tests (add_subdirectory, find_package, FetchContent) validate library functionality on Windows.
=======
- refac
- cmake
- remove ineffective Catch2 cache variables from CMakeLists
- avoid std::byte in Catch2 assertions to prevent MSVC linker errors
- add diagnostic messages for Catch2 patching
- patch Catch2 CMakeLists to force config on MSVC
- set Catch2 config unconditionally before project()
- disable Catch2 CPP17_STRING_VIEW and CPP17_BYTE on MSVC
- set Catch2 config as CACHE variables before FetchContent
- disable CPP17_STRING_VIEW and CPP17_BYTE StringMakers on MSVC
- pass CATCH_CONFIG_NO_CPP17_* as compile definitions to test target
- disable Catch2 C++17 string_view/byte on MSVC to avoid linker errors
- use inline CMake commands for Windows/MSVC jobs
- sign conversion warnings in binary_event_traits
- correct example paths and Windows warnings policy
- resolve platform-specific workflow failures
- validate project workflow jobs
- use valid container image expressions
>>>>>>> 72bbf65371c1106b212e638abb9ada5bb3658b98

## [0.4.0] - 2026-11-20

### Added

- Implemented `HTTPBus` as a bounded in-memory HTTP relay with server/client roles, POST publishing, and client polling.
- Implemented the shared `FramedBus` local delivery, serialization, and receive path.

## [0.3.0] - 2026-11-18

### Added

- add script updater with auto-sync and pre-flight checks

## [0.2.0] - 2026-11-18

### Added

- Initial release

## [0.1.0] - 2026-11-18

### Added

- `IEventBus`: transport-agnostic bus with typed, non-virtual `publish<T>()` / `subscribe<T>()` over type-erased virtuals; fire-and-forget publish, coroutine handlers (`EventHandler<T>` = `std::function<Task<void>(const T&)>`), `drain()`.
- `EventTypeId` (FNV-1a hash of the declared name), `EventTypeInfo`, `EventTraits<T>`, `NamedEvent` / `SerializableEvent` concepts.
- `InProcessBus`: in-memory transport, no serialization, one shared event object per publish, thread-safe, concurrent handlers.
- `Subscription` (RAII), `EventBusConfig`, `EventBusFactory`, `TransportKind`, and the exceptions `TransportNotAvailable`, `TransportNotImplemented`, `NotSerializable`.
- `FramedBus` foundation and TODO stubs for `UnixSocketBus`, `TCPServerBus`, `HTTPBus`, `WebSocketBus`, `RedisBus`, `ZeroMQBus`, `RabbitMQBus`, `KafkaBus` (CMake options `TOBUS_ENABLE_*`).
- Example `01_in_process` and Catch2 tests.
- CI (`.github/workflows/`): build/test matrix (clang-18 + libc++, g++-15), sanitizers (ASan+UBSan, TSan), 30x repeat of the concurrency tests, clang-format/clang-tidy, coverage, and downstream-consumer checks (`add_subdirectory`, `find_package`, `FetchContent`).
- `tobus_concurrency` CTest entry running the `[threads]` test cases.

### Changed

- A failure with no `EventBusConfig::on_error` is now written to `stderr` instead of being dropped silently; install an empty handler to silence it.
- `publish<T>()` throws `std::logic_error` if another event type with the same `EventTypeId` has subscribers on the bus (previously the payload could be delivered to the wrong type).
- `TransportKind` has an explicit `std::uint8_t` underlying type.
- `detail/dispatcher.hpp` moved from `include/` to `src/detail/`: it is no longer installed or exported.
- `.clang-tidy` is now a narrow check set that fails on any finding.

### Fixed

- `VERSION` was hidden by the template's `version` rule in `.gitignore` (case-insensitive matching), so a `FetchContent` clone could not configure.
- `compile_commands.json` is now exported (clang-tidy needs it).
