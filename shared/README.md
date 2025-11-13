# Gecko Compression

Lightweight, bitonal image compression static C++ library that was designed with WASM-compatibility in mind. Built for The Gecko Project.

## Installation

Gecko requires the following to build:
- CMake version 3.16 or above
- A compiler which supports C++ 20 features

First, clone the reposity. Gecko can be built and configured for a variety of different environments and build systems. To configure and build a x64 Release for Visual Studio 2022, run:

```bash
cmake -B out/build -S . -G "Visual Studio 17 2022" -A x64 && cmake --build out/build --config Release
```

Subsequent builds that don't require reconfiguring cmake can be simplified:

```bash
cmake --build out/build --config Release
```

### Running Tests

Gecko uses GoogleTest framework, and integrates with `ctest` to run tests in a configuration-agnostic way. To execute all tests for a Release build, run:

```bash
ctest --test-dir build -C Release
```

If you've configured your project for Visual Studio, the tests can alternatively be discovered and run via the Test Explorer.


## Usage

For now, Gecko's compression library currently only supports encoding from / to 3-channel bitmaps. More specifically, it only supports encoding to uncompressed, positive-height, 24bpp `.bmp` files that follow the BITMAPINFOHEADER header format. Indexed-color bitmaps (with color palettes) are not supported.

Although these limitations may seem overly strict, Gecko is only really designed to be used by, well, Gecko.

### Samples

##### Bitonal compression of a BMP:
```cpp
#include <optional>
#include <cstddef>
#include <vector>
#include "CompressedBitonal.h"
#include "UncompressedBitonal.h"
using namespace Gecko::Compression;

...

std::vector<uint8_t> bmpBytes{}; // Load uncompressed bytes

std::optional<UncompressedBitonal> uncompressed;
std::optional<CompressedBitonal> compressed;
std::optional<std::vector<uint8_t>> compressedBytes;

uncompressed = UncompressedBitonal::TryReadFromBuffer(bmpBytes, UncompressedBitonal::StorageFormat::BMPStrict24);

if (uncompressed)
  compressed = Encoder::TryCompressBitonal(*uncompressed);

if (compressed)
  compressedBytes = CompressedBitonal::TryWriteToBuffer(*compressed, CompressedBitonal::StorageFormat::BDC);

if (compressedBytes)
  ; // Save compressed bytes
```
