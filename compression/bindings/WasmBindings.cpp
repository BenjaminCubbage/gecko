#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "CompressedBitonal.h"
#include "UncompressedBitonal.h"
#include "Encoder.h"
#include "Decoder.h"


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
            .class_function("TryReadFromBuffer", &CompressedBitonal::TryReadFromBuffer);

        emscripten::register_optional<CompressedBitonal>();

        /*
         *  Decoder
         */
        emscripten::class_<Decoder>("Decoder")
            .class_function("TryDecompressBitonal", &Decoder::TryDecompressBitonal);

        /*
         *  Encoder
         */
        emscripten::class_<Encoder>("Encoder")
            .class_function("TryCompressBitonal", &Encoder::TryCompressBitonal);
    }
}