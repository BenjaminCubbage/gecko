#include "gecko/CompressedBitonal.h"
#include "gecko/Decoder.h"
#include "gecko/Encoder.h"
#include "gecko/UncompressedBitonal.h"
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
         *  UncompressedBitonal
         */

        emscripten::enum_<UncompressedBitonal::StorageFormat>("UncompressedBitonal_StorageFormat")
            .value("BMPStrict24", UncompressedBitonal::StorageFormat::BMPStrict24);

        emscripten::class_<UncompressedBitonal>("UncompressedBitonal")
            .constructor<std::vector<uint8_t>, size_t, size_t>()
            .class_function("TryReadFromBuffer", &UncompressedBitonal::TryReadFromBuffer)
            .class_function("TryWriteToBuffer", &UncompressedBitonal::TryWriteToBuffer)
            .function("GetBGR", &UncompressedBitonal::GetBGR)
            .function("GetWidth", &UncompressedBitonal::GetWidth)
            .function("GetHeight", &UncompressedBitonal::GetHeight);

        emscripten::register_optional<UncompressedBitonal>();

        /*
         *  CompressedBitonal
         */

        emscripten::enum_<CompressedBitonal::StorageFormat>("CompressedBitonal_StorageFormat")
            .value("GIB", CompressedBitonal::StorageFormat::GIB);

        emscripten::class_<CompressedBitonal>("CompressedBitonal")
            .class_function("TryWriteToBuffer", &CompressedBitonal::TryWriteToBuffer)
            .class_function("TryReadFromBuffer", &CompressedBitonal::TryReadFromBuffer)
            .function("GetWidth", &CompressedBitonal::GetWidth)
            .function("GetHeight", &CompressedBitonal::GetHeight);;

        emscripten::register_optional<CompressedBitonal>();

        /*
         *  Decoder
         */

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
        });

        emscripten::class_<Decoder<ImageWriter>>("Decoder")
            .class_function("TryDecompress",
                (bool (*)(CompressedBitonal, int)) [] (CompressedBitonal compressed, int cbIntPtr) {
                    return Decoder<ImageWriter>::TryDecompress(compressed, (void*)cbIntPtr);
                });

        /*
         *  Encoder
         */

        using ImageReader = decltype([] (void* context, void*, size_t px, size_t py) -> bool {
            return reinterpret_cast<int (*)(int, int)>(context)(px, py);
        });

        emscripten::class_<Encoder<ImageReader>>("Encoder")
            .class_function("TryCompress",
                (std::optional<CompressedBitonal> (*)(int, int, int))
                [] (int cbIntPtr, int width, int height) {
                    return Encoder<ImageReader>::TryCompress(
                        (size_t)width,
                        (size_t)height,
                        (void*)cbIntPtr);
                });
    }
}