#include "gtest/gtest.h"
#include "BitStream.h"

namespace Gecko::Compression::Test
{

	TEST(BitStream, ProgressAndReadAtByteBoundaries)
	{
		BitStream stream(std::vector<uint8_t>({ uint8_t(0x01), uint8_t(0x01), uint8_t(0x01), uint8_t(0x01) }));

		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(8)); stream.StepForward(8);
	}

	TEST(BitStream, ProgressAndReadAt4BitOffsets)
	{
		BitStream stream(std::vector<uint8_t>({ uint8_t(0x01), uint8_t(0x01), uint8_t(0x01), uint8_t(0x01) }));

		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(4)); stream.StepForward(4);
	}

	TEST(BitStream, ProgressAndReadAt3BitOffsets)
	{
		BitStream stream(std::vector<uint8_t>({ uint8_t(0x11), uint8_t(0x11), uint8_t(0x11) }));

		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x04), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x02), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x04), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x02), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(3)); stream.StepForward(3);
	}

	TEST(BitStream, ProgressAndRead1BitAtATime)
	{
		BitStream stream(std::vector<uint8_t>({ uint8_t(0b01010101) }));

		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(1)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x01), stream.Peek(1)); stream.StepForward(1);
	}

	TEST(BitStream, ProgressAndReadZeroesFromEmptyStream)
	{
		BitStream stream(std::vector<uint8_t>{ });

		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(8));  stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(16)); stream.StepForward(2);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(32)); stream.StepForward(1);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(5));  stream.StepForward(24);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(1));  stream.StepForward(0);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(12)); stream.StepForward(32);
		EXPECT_EQ(static_cast<uint32_t>(0x00), stream.Peek(32)); stream.StepForward(128);
	}

	TEST(BitStream, PassInvalidArgument2ToPeak)
	{
		int exceptionsThrown = 0;
		BitStream stream(std::vector<uint8_t>{ });

		try									 { stream.Peek(-1); }
		catch (const std::invalid_argument&) { ++exceptionsThrown; }
			
		try									 { stream.Peek(33); }
		catch (const std::invalid_argument&) { ++exceptionsThrown; }
			
		EXPECT_EQ(exceptionsThrown, 2);
	}

	TEST(BitStream, Append1ByteAtATime)
	{
		BitStream stream(std::vector<uint8_t>({ uint8_t(0b10101010) }));

		stream.PushBack(0b01010101, 8);
		stream.PushBack(0b10101010, 8);
		stream.PushBack(0b01010101, 8);

		EXPECT_EQ(static_cast<uint32_t>(0b10101010), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0b01010101), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0b10101010), stream.Peek(8)); stream.StepForward(8);
		EXPECT_EQ(static_cast<uint32_t>(0b01010101), stream.Peek(8)); stream.StepForward(8);
	}

	TEST(BitStream, Append3BitsAtATime)
	{
		BitStream stream;

		stream.PushBack(0b001, 3);
		stream.PushBack(0b010, 3);
		stream.PushBack(0b100, 3);
		stream.PushBack(0b011, 3);
		stream.PushBack(0b111, 3);
		stream.PushBack(0b101, 3);
		stream.PushBack(0b101, 3);

		EXPECT_EQ(static_cast<uint32_t>(0b001), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b010), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b100), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b011), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b111), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b101), stream.Peek(3)); stream.StepForward(3);
		EXPECT_EQ(static_cast<uint32_t>(0b101), stream.Peek(3)); stream.StepForward(3);
	}

	TEST(BitStream, Append31BitsAtATime)
	{
		BitStream stream;
		stream.PushBack(0b1010101010011110101010010101111, 31);
		stream.PushBack(0b1010100101010101011010101010111, 31);
		stream.PushBack(0b1011110101010110001110111011000, 31);
			
		EXPECT_EQ(static_cast<uint32_t>(0b1010101010011110101010010101111), stream.Peek(31)); stream.StepForward(31);
		EXPECT_EQ(static_cast<uint32_t>(0b1010100101010101011010101010111), stream.Peek(31)); stream.StepForward(31);
		EXPECT_EQ(static_cast<uint32_t>(0b1011110101010110001110111011000), stream.Peek(31)); stream.StepForward(31);
	}
}
