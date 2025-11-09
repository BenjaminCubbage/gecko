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
        BitStream bs({ std::byte{0xAB}, std::byte{0xCD} });
        CompressedBitonal compressed(bs, 5, 5);

        EXPECT_THROW(
            CompressedBitonal::TryWriteToBuffer(compressed, static_cast<CompressedBitonal::StorageFormat>(999)),
            std::invalid_argument);
    }

    TEST(CompressedBitonal, TryWriteToBuffer_EmptyBitstream)
    {
        BitStream empty(std::vector<std::byte>{ });
        CompressedBitonal compressed(empty, 5, 5);

        auto result = CompressedBitonal::TryWriteToBuffer(compressed, CompressedBitonal::StorageFormat::BDC);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryWriteAndRead_Success)
    {
        std::vector<std::byte> fakeBytes = { std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC} };
        BitStream bs(fakeBytes);
        CompressedBitonal original(bs, 8, 4);

        auto bufferOpt = CompressedBitonal::TryWriteToBuffer(original, CompressedBitonal::StorageFormat::BDC);
        ASSERT_TRUE(bufferOpt.has_value());
        const auto& buffer = *bufferOpt;

        auto readOpt = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::BDC);
        ASSERT_TRUE(readOpt.has_value());

        const auto& decoded = *readOpt;
        EXPECT_EQ(decoded.GetWidth(), 8u);
        EXPECT_EQ(decoded.GetHeight(), 4u);

        const auto span = decoded.GetBitstream().Bytes();

        ASSERT_EQ(span.size(), fakeBytes.size());
        for (size_t i = 0; i < span.size(); ++i)
        {
            EXPECT_EQ(std::to_integer<uint8_t>(span[i]), std::to_integer<uint8_t>(fakeBytes[i]))
                << "Byte mismatch at index " << i;
        }
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidFormat)
    {
        std::vector<std::byte> buffer(20, std::byte{ 0 });
        buffer[0] = std::byte{ 'B' };
        buffer[1] = std::byte{ 'C' };

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, static_cast<CompressedBitonal::StorageFormat>(999));
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidSignature)
    {
        std::vector<std::byte> buffer(20, std::byte{ 0 });
        buffer[0] = std::byte{ 'X' };
        buffer[1] = std::byte{ 'Y' };

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::BDC);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_InvalidHeaderSize)
    {
        std::vector<std::byte> buffer(20, std::byte{ 0 });
        buffer[0] = std::byte{ 'B' };
        buffer[1] = std::byte{ 'C' };
        // header size != 14
        uint32_t wrongHeaderSize = 10;
        std::memcpy(buffer.data() + 2, &wrongHeaderSize, 4);

        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::BDC);
        EXPECT_FALSE(result.has_value());
    }

    TEST(CompressedBitonal, TryReadFromBuffer_ZeroDimensions)
    {
        std::vector<std::byte> buffer(20, std::byte{ 0 });
        buffer[0] = std::byte{ 'B' };
        buffer[1] = std::byte{ 'C' };
        uint32_t headerSize = 14;
        std::memcpy(buffer.data() + 2, &headerSize, 4);
        // width and height = 0
        auto result = CompressedBitonal::TryReadFromBuffer(buffer, CompressedBitonal::StorageFormat::BDC);
        EXPECT_FALSE(result.has_value());
    }

}