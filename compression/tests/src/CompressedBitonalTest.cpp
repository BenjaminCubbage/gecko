#include "gtest/gtest.h"
#include "CompressedBitonal.h"
#include "BitStream.h"
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <cstring>

namespace Gecko::Compression::Test 
{
    TEST(CompressedBitonal, TryWriteToBuffer_InvalidFormat)
    {
        BitStream bs({ uint8_t{0xAB}, uint8_t{0xCD} });
        CompressedBitonal compressed(bs, 5, 5);

        EXPECT_THROW(
            CompressedBitonal::TryWriteToBuffer(compressed, static_cast<CompressedBitonal::StorageFormat>(999)),
            std::invalid_argument);
    }

    TEST(CompressedBitonal, TryWriteToBuffer_EmptyBitstream)
    {
        BitStream empty(std::vector<uint8_t>{ });
        CompressedBitonal compressed(empty, 5, 5);

        auto result = CompressedBitonal::TryWriteToBuffer(compressed, CompressedBitonal::StorageFormat::GIB);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryWriteAndRead_Success)
    {
        std::vector<uint8_t> fakeBytes = { uint8_t{0xAA}, uint8_t{0xBB}, uint8_t{0xCC} };
        BitStream bs(fakeBytes);
        CompressedBitonal original(bs, 8, 4);

        auto bufferOpt = CompressedBitonal::TryWriteToBuffer(original, CompressedBitonal::StorageFormat::GIB);
        ASSERT_TRUE(bufferOpt.has_value());
        const auto& buffer = *bufferOpt;

        auto readOpt = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::GIB);
        ASSERT_TRUE(readOpt.has_value());

        const auto& decoded = *readOpt;
        EXPECT_EQ(decoded.GetWidth(), 8u);
        EXPECT_EQ(decoded.GetHeight(), 4u);

        const auto span = decoded.GetBitstream().GetBytes();

        ASSERT_EQ(span.size(), fakeBytes.size());
        for (size_t i = 0; i < span.size(); ++i)
            EXPECT_EQ(span[i], fakeBytes[i]) << "Byte mismatch at index " << i;
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidFormat)
    {
        std::vector<uint8_t> buffer(20, uint8_t{ 0 });
        buffer[0] = uint8_t{ 'B' };
        buffer[1] = uint8_t{ 'C' };

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, static_cast<CompressedBitonal::StorageFormat>(999));
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidSignature)
    {
        std::vector<uint8_t> buffer(20, uint8_t{ 0 });
        buffer[0] = uint8_t{ 'X' };
        buffer[1] = uint8_t{ 'Y' };

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::GIB);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidHeaderSize)
    {
        std::vector<uint8_t> buffer(20, uint8_t{ 0 });
        buffer[0] = uint8_t{ 'B' };
        buffer[1] = uint8_t{ 'C' };
        // header size != 14
        uint32_t wrongHeaderSize = 10;
        std::memcpy(buffer.data() + 2, &wrongHeaderSize, 4);

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::GIB);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_ZeroDimensions)
    {
        std::vector<uint8_t> buffer(20, uint8_t{ 0 });
        buffer[0] = uint8_t{ 'B' };
        buffer[1] = uint8_t{ 'C' };
        uint32_t headerSize = 14;
        std::memcpy(buffer.data() + 2, &headerSize, 4);
        // width and height = 0
        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::GIB);
        EXPECT_FALSE(result.has_value());
    }

}