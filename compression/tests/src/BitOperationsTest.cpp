#include "gtest/gtest.h"
#include "BitOperations.h"

namespace BDCompressTest
{
	TEST(BitOperations, BitOperationsSwapEndianness32)
	{
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x00000000)), static_cast<uint32_t>(0x00000000));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x01000000)), static_cast<uint32_t>(0x00000001));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x00FF0000)), static_cast<uint32_t>(0x0000FF00));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x111FF111)), static_cast<uint32_t>(0x11F11F11));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x12345678)), static_cast<uint32_t>(0x78563412));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0xFFFFFFFF)), static_cast<uint32_t>(0xFFFFFFFF));
		EXPECT_EQ(BitOperations::SwapEndianness32(static_cast<uint32_t>(0x0FA00000)), static_cast<uint32_t>(0x0000A00F));
	}

	TEST(BitOperations, BitOperationsSwapEndianness16)
	{
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x0000)), static_cast<uint16_t>(0x0000));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x0100)), static_cast<uint16_t>(0x0001));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x00FF)), static_cast<uint16_t>(0xFF00));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x111F)), static_cast<uint16_t>(0x1F11));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x1234)), static_cast<uint16_t>(0x3412));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0xFFFF)), static_cast<uint16_t>(0xFFFF));
		EXPECT_EQ(BitOperations::SwapEndianness16(static_cast<uint16_t>(0x0FA0)), static_cast<uint16_t>(0xA00F));
	}
}
