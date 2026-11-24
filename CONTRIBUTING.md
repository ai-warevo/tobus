# Contributing to aw::tobus

Thank you for your interest in contributing to **aw::tobus** — a transport-agnostic event bus for C++20.

## 🚀 Quick Start

1. **Fork** the repository
2. **Clone** your fork:
   ```bash
   git clone https://github.com/<your-username>/tobus.git
   cd tobus
   ```
3. **Create a feature branch**:
   ```bash
   git checkout -b feature/amazing-feature
   ```
4. **Build and test**:
   ```bash
   cmake -B build/dev -DCMAKE_BUILD_TYPE=Debug
   cmake --build build/dev --parallel
   ctest --test-dir build/dev --output-on-failure --parallel 4
   ```

## 📋 Development Discipline

### Coding Standards

- **C++20 idiomatic style**: concepts/requires over SFINAE, coroutines, `std::jthread`, RAII everywhere
- **Naming conventions**: types `PascalCase`, functions/variables `snake_case`, private members `trailing_underscore_`
- **Formatting**: 4 spaces, 120 cols, attached braces (see `.clang-format`, `.editorconfig`)
- **Headers**: include guards (`#ifndef AW_TOBUS_<PATH>_HPP` / `#define` / `#endif  // AW_TOBUS_<PATH>_HPP`), include groups (own header → `<aw/tobus/...>` → `<std>`), blank line between groups

### Architecture Rules

- **Namespaces map to directories**:
  - `aw::tobus` → `include/aw/tobus/`, `src/`
  - `aw::tobus::transports` → `.../transports/`
  - `*::detail` → `src/detail/` (internal, not public API)
- **Transport-agnostic principle**: `IEventBus` is the single interface; every transport implements it
- **Transport stubs**: OFF by default (`DTOBUS_ENABLE_*`); compile only when explicitly enabled
- **One logical concept per header**; `tobus.hpp` is the umbrella header

### Testing Requirements

- **Every new feature needs tests** in `tests/` (tag with `[area]`)
- **Tests must pass** under ASan/UBSan/TSan with 120s ctest timeout
- **Test concurrency deterministically** (latches, atomics, `std::binary_semaphore`), not with sleeps

### Documentation

- **Every public entity gets Doxygen** (`@class`, `@brief`, `@tparam`, `@param[in]`, `@return`, `@throws`)
- **Inline comments**: minimal, only for non-obvious logic or the *why* behind a choice
- **Update README** when public behaviour changes

## 🔄 Pull Request Process

1. **Update CHANGELOG.md** — add every user-visible change under `## [Unreleased]` in the same commit
2. **Run local tests** before pushing:
   ```bash
   cmake --build build/dev --parallel
   ctest --test-dir build/dev --output-on-failure --parallel 4
   ```
3. **Run clang-tidy**:
   ```bash
   make lint
   ```
4. **Request review** from maintainers
5. **Minimum one approving review** required
6. **All CI checks must pass** (build matrix, tests, sanitizers, consumer integration)

## 📝 Conventional Commits

Use [Conventional Commits](https://www.conventionalcommits.org/) format:

```
type(scope): description

- `feat(transports): add HTTPBus with frame framing`
- `fix(dispatcher): fix event ordering on concurrent publish`
- `docs(readme): update integration example`
- `refactor(detail): simplify dispatcher state machine`
- `test(in_process): add concurrent publish/subscribe stress test`
```

**Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`

## 🧪 Adding Tests

1. Create test file in `tests/` (e.g., `test_new_transport.cpp`)
2. Register in `tests/CMakeLists.txt`
3. Tag tests with area: `TEST_CASE("[transport][http]", "[transport]")`
4. Cover: success paths, failure modes, edge cases, transport switching

## 🔧 Debugging

- **Sanitizers**: CI runs ASan/UBSan and TSan; enable locally:
  ```bash
  cmake -B build/san -DCMAKE_BUILD_TYPE=RelWithDebInfo -DAW_TPL_SANITIZE=address
  cmake --build build/san --parallel
  ctest --test-dir build/san --output-on-failure
  ```
- **Coverage**: generate HTML report:
  ```bash
  cmake -B build/cov -DCMAKE_BUILD_TYPE=Debug -DAW_TPL_ENABLE_COVERAGE=ON
  cmake --build build/cov --parallel
  ctest --test-dir build/cov --output-on-failure
  gcovr -r build/cov/ --filter "include/aw/tobus/" --filter "src/" --html-details htmlcov/index.html
  ```

## ❓ Questions?

- Open an [Issue](https://github.com/ai-warevo/tobus/issues) with the `question` label
- Check existing issues and [README](README.md) for common patterns

## 🙏 Thank You!

Your contributions make aw::tobus better for everyone. Every bug report, feature request, and pull request is valued.
