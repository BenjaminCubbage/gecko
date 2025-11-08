#include "gtest/gtest.h"
#include "UncompressedBitonal.h"

namespace
{
    // Helper: build a minimal valid BMPStrict24 buffer (little-endian)
    std::vector<std::byte> MakeValidBmpStrict24Buffer(uint32_t w, uint32_t h)
    {
        constexpr int HeaderSize = 54;
        const uint32_t rowBytesLen = (w * 3 + 3) & ~3;
        const uint32_t pixelBytes = rowBytesLen * h;
        std::vector<std::byte> buf(HeaderSize + pixelBytes, std::byte{ 0 });

        auto w32 = [&](size_t off, uint32_t v) {
            buf[off + 0] = std::byte(v & 0xFF);
            buf[off + 1] = std::byte((v >> 8) & 0xFF);
            buf[off + 2] = std::byte((v >> 16) & 0xFF);
            buf[off + 3] = std::byte((v >> 24) & 0xFF);
        };

        auto w16 = [&](size_t off, uint16_t v) {
            buf[off + 0] = std::byte(v & 0xFF);
            buf[off + 1] = std::byte((v >> 8) & 0xFF);
        };

        // BITMAPFILEHEADER
        buf[0] = std::byte('B');
        buf[1] = std::byte('M');
        w32(2, static_cast<uint32_t>(buf.size())); // file size
        w32(10, HeaderSize);                       // pixel offset

        // BITMAPINFOHEADER (40 bytes)
        w32(14, 40);
        w32(18, w);
        w32(22, h);
        w16(26, 1);
        w16(28, 24);
        w32(34, pixelBytes);

        std::fill(buf.begin() + HeaderSize, buf.end(), std::byte{ 0x7F });
        return buf;
    }
}

namespace BDCompressTest 
{
    TEST(UncompressedBitonal, Construct_Success)
    {
        std::vector<std::byte> pixels(3 * 3 * 3, std::byte{ 255 });
        UncompressedBitonal img(pixels, 3, 3);

        EXPECT_EQ(img.GetWidth(), 3u);
        EXPECT_EQ(img.GetHeight(), 3u);
        EXPECT_TRUE(img.GetPixel(0, 0));
        EXPECT_TRUE(img.GetPixel(2, 2));
    }
    
    TEST(UncompressedBitonal, Constructor_InvalidBGRSize)
    {
        // 2x2 image requires 2*2*3 = 12 bytes, but we give only 10
        std::vector<std::byte> malformedBgr(10, std::byte{ 0 });
        EXPECT_THROW(UncompressedBitonal badImg(malformedBgr, 2, 2), std::invalid_argument);
    }

    TEST(UncompressedBitonal, TryWriteToBuffer_InvalidFormat)
    {
        std::vector<std::byte> pixels(3 * 3 * 3, std::byte{ 0 });
        UncompressedBitonal img(pixels, 3, 3);

        EXPECT_THROW(
            UncompressedBitonal::TryWriteToBuffer(img, static_cast<UncompressedBitonal::StorageFormat>(999)),
            std::invalid_argument);
    }

    TEST(UncompressedBitonal, TryWriteToBuffer_InvalidDimensions)
    {
        std::vector<std::byte> pixels;
        UncompressedBitonal img(pixels, 0, 0);

        auto result = UncompressedBitonal::TryWriteToBuffer(img, UncompressedBitonal::StorageFormat::BMPStrict24);
        EXPECT_FALSE(result.has_value());
    }

    TEST(UncompressedBitonal, TryWriteAndRead_Success)
    {
        const uint32_t w = 2;
        const uint32_t h = 2;
        std::vector<std::byte> pixels(w * h * 3, std::byte{ 255 });

        UncompressedBitonal img(pixels, w, h);
        auto bufferOpt = UncompressedBitonal::TryWriteToBuffer(img, UncompressedBitonal::StorageFormat::BMPStrict24);
        ASSERT_TRUE(bufferOpt.has_value());

        auto readOpt = UncompressedBitonal::TryReadFromBuffer(*bufferOpt, UncompressedBitonal::StorageFormat::BMPStrict24);
        ASSERT_TRUE(readOpt.has_value());

        const auto& decoded = *readOpt;
        EXPECT_EQ(decoded.GetWidth(), static_cast<size_t>(w));
        EXPECT_EQ(decoded.GetHeight(), static_cast<size_t>(h));
        EXPECT_TRUE(decoded.GetPixel(0, 0));
        EXPECT_TRUE(decoded.GetPixel(1, 1));
    }

    TEST(UncompressedBitonal, TryReadFromBuffer_InvalidHeader)
    {
        auto buf = MakeValidBmpStrict24Buffer(2, 2);
        buf[0] = std::byte{ 'B' };
        buf[1] = std::byte{ 'N' };

        auto result = UncompressedBitonal::TryReadFromBuffer(buf, UncompressedBitonal::StorageFormat::BMPStrict24);
        EXPECT_FALSE(result.has_value());
    }

    TEST(UncompressedBitonal, TryReadFromBuffer_InvalidHeaderSize)
    {
        auto buf = MakeValidBmpStrict24Buffer(2, 2);

        // Corrupt only the header size
        uint32_t wrongHeader = 12;
        std::memcpy(buf.data() + 14, &wrongHeader, 4);

        auto result = UncompressedBitonal::TryReadFromBuffer(buf, UncompressedBitonal::StorageFormat::BMPStrict24);
        EXPECT_FALSE(result.has_value());
    }

    TEST(UncompressedBitonal, TryReadFromBuffer_InvalidBpp)
    {
        auto buf = MakeValidBmpStrict24Buffer(3, 1);

        uint16_t badBpp = 8;
        std::memcpy(buf.data() + 28, &badBpp, 2);

        auto result = UncompressedBitonal::TryReadFromBuffer(buf, UncompressedBitonal::StorageFormat::BMPStrict24);
        EXPECT_FALSE(result.has_value());
    }

    TEST(UncompressedBitonal, TryReadFromBuffer_PixelsTooShort)
    {
        auto buf = MakeValidBmpStrict24Buffer(5, 2);
        buf.pop_back(); // shorten by one byte

        auto result = UncompressedBitonal::TryReadFromBuffer(buf, UncompressedBitonal::StorageFormat::BMPStrict24);
        EXPECT_FALSE(result.has_value());
    }

    TEST(UncompressedBitonal, TryReadFromBuffer_InvalidFormatEarlyExit)
    {
        auto buf = MakeValidBmpStrict24Buffer(2, 2);

        auto result = UncompressedBitonal::TryReadFromBuffer(
            buf, static_cast<UncompressedBitonal::StorageFormat>(999));
        EXPECT_FALSE(result.has_value());
    }
}