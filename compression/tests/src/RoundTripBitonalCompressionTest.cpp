#include "gtest/gtest.h"
#include "UncompressedBitonal.h"
#include "CompressedBitonal.h"
#include "Encoder.h"
#include "Decoder.h"
#include <fstream>
#include <filesystem>

namespace
{
    static std::vector<std::byte> ReadFileBytes(const std::filesystem::path& path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("Failed to open: " + path.string());
        std::vector<char> temp((std::istreambuf_iterator<char>(in)), {});
        std::vector<std::byte> bytes(temp.size());
        std::memcpy(bytes.data(), temp.data(), temp.size());
        return bytes;
    }
}

namespace Gecko::Compression::Test
{
    TEST(RoundTripBitonalCompression, FullImage)
    {
        const std::filesystem::path inputBmp = "assets/TestBitonal.bmp";

        // --- Load BMP ---
        const auto bmpBytes = ReadFileBytes(inputBmp);
        ASSERT_FALSE(bmpBytes.empty()) << "Test image missing or empty.";

        auto uOpt = UncompressedBitonal::TryReadFromBuffer(
            bmpBytes, UncompressedBitonal::StorageFormat::BMPStrict24);
        ASSERT_TRUE(uOpt.has_value()) << "Failed to parse BMP input.";
        const auto& uncompressed = *uOpt;

        // --- Compress ---
        auto cOpt = Encoder::TryCompressBitonal(uncompressed);
        ASSERT_TRUE(cOpt.has_value()) << "Compression failed.";

        auto cBufOpt = CompressedBitonal::TryWriteToBuffer(
            *cOpt, CompressedBitonal::StorageFormat::BDC);
        ASSERT_TRUE(cBufOpt.has_value()) << "Failed to serialize compressed buffer.";

        // --- Reload and Decompress ---
        auto cReadOpt = CompressedBitonal::TryReadFromBuffer(
            *cBufOpt, CompressedBitonal::StorageFormat::BDC);
        ASSERT_TRUE(cReadOpt.has_value()) << "Failed to parse compressed data.";

        auto uOutOpt = Decoder::TryDecompressBitonal(*cReadOpt);
        ASSERT_TRUE(uOutOpt.has_value()) << "Decompression failed.";
        const auto& recon = *uOutOpt;

        // --- Verify structure ---
        EXPECT_EQ(recon.GetWidth(), uncompressed.GetWidth());
        EXPECT_EQ(recon.GetHeight(), uncompressed.GetHeight());

        // --- Verify pixel-level equivalence ---
        const auto& a = uncompressed.GetBGR();
        const auto& b = recon.GetBGR();
        ASSERT_EQ(a.size(), b.size());

        bool different = false;
        size_t differentAt = 0;

        for (size_t i = 0; i < a.size(); ++i)
            if (a[i] != b[i])
            {
                different = true;
				differentAt = i;
                break;
			}

		EXPECT_FALSE(different) << "Decompressed image differs from original at byte " << differentAt << "/" << a.size() << ".";
    }
}