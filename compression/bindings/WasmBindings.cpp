#include "gecko/BitStream.h"
#include "gecko/Decode.h"
#include "gecko/Encode.h"
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <vector>


namespace Gecko::Compression::WasmBindings
{
    EMSCRIPTEN_BINDINGS(CompressionModule)
    {
        emscripten::register_vector<uint8_t>("VectorUint8");
        emscripten::register_optional<std::vector<uint8_t>>();

        /*
         *  Decode
         */

        using HeaderWriter = decltype([] (
                void*,
                void*,
                const Header&) {
            return true;
        });

        using ImageWriter = decltype([] (
                void* context,
                void*,
                size_t pixelY,
                size_t pixelXStart,
                size_t pixelXEnd,
                bool white) {
            reinterpret_cast<void (*)(int, int, int, int)>(context)(
                (int)pixelY,
                (int)pixelXStart,
                (int)pixelXEnd,
                white);

            return true;
        });

        emscripten::class_<Decode<HeaderWriter, ImageWriter>>("Decode")
            .class_function("TryDecompress",
                (bool (*)(std::vector<uint8_t>, int)) [] (
                        std::vector<uint8_t> buffer,
                        int cbIntPtr) {
                    BitStream bs{ buffer };
                    return Decode<HeaderWriter, ImageWriter>::TryDecompress(bs, (void*)cbIntPtr);
                });

        /*
         *  Encode
         */

        using ImageReader = decltype([] (void* context, void*, size_t px, size_t py) -> bool {
            return reinterpret_cast<int (*)(int, int)>(context)(px, py);
        });

        emscripten::class_<Encode<ImageReader>>("Encode")
            .class_function("TryCompress",
                (std::optional<std::vector<uint8_t>> (*)(int, int, int))
                [] (int cbIntPtr, int width, int height) {
                    auto bs = Encode<ImageReader>::TryCompress(
                        (size_t)width,
                        (size_t)height,
                        (void*)cbIntPtr);

                    return bs
                        ? std::optional{ std::move(bs->Release()) }
                        : std::nullopt;
                });
    }
}