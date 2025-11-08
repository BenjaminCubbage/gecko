# Gecko

Lightweight, bitonal image compression static C++ library that was designed with WASM-compatibility in mind. Built for The Gecko Project.

## Installation

Gecko requires the following to build:
- CMake version 3.16 or above
- A compiler which supports C++ 20 features

First, clone the reposity. Gecko can be built and configured for a variety of different environments and build systems. To configure and build a x64 Release for Visual Studio 2022, run:

```bash
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 && cmake --build build --config Release
```

Subsequent builds that don't require reconfiguring cmake can be simplified:

```bash
cmake --build build --config Release
```

### Running Tests

Gecko uses GoogleTest framework, and integrates with `ctest` to run tests in a configuration-agnostic way. To execute all tests for a Release build, run:

```bash
ctest --test-dir build -C Release
```

If you've configured your project for Visual Studio, the tests can alternatively be discovered and run via the Test Explorer.
