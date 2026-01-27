#ifndef DEBUG_TRACE
#define DEBUG_TRACE
#endif

#include "gecko/BitStream.h"
#include "gecko/CodeWords.h"
#include "gecko/Decode.h"
#include "gecko/Encode.h"
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <optional>
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
                void* cbHeader,
                void* cbWriter,
                const Header& header) {
            return reinterpret_cast<int (*)(int, int)>(cbHeader)(
                header.width,
                header.height) != 0;
        });

        using ImageWriter = decltype([] (
                void* cbHeader,
                void* cbWriter,
                size_t pixelY,
                size_t pixelXStart,
                size_t pixelXEnd,
                bool white) {
            reinterpret_cast<void (*)(int, int, int, int)>(cbWriter)(
                (int)pixelY,
                (int)pixelXStart,
                (int)pixelXEnd,
                white);

            return true;
        });

        emscripten::class_<Decode<HeaderWriter, ImageWriter>>("Decode")
            .class_function("TryDecompress",
                (bool (*)(std::vector<uint8_t>, int, int)) [] (
                        std::vector<uint8_t> buffer,
                        int cbHeaderIntPtr,
                        int cbWriterIntPtr) {
                    BitStream bs{ buffer };
                    return Decode<HeaderWriter, ImageWriter>::TryDecompress(
                        bs,
                        (void*)cbHeaderIntPtr,
                        (void*)cbWriterIntPtr);
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