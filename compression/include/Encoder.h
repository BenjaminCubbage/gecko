#pragma once
#include "CompressedBitonal.h"
#include "UncompressedBitonal.h"
#include "BitStream.h"


namespace Gecko::Compression
{
	/*
	 A `vPixel` here refers to a virtual pixel, which is a coordinate which may or may not map
	 to a real point on the bitmap. If it does not map to a real point, the coordinate is
	 always considered white.
	 */

	class Encoder
	{
	public:
		static std::optional<CompressedBitonal> TryCompressBitonal(const UncompressedBitonal& uncompressed);

	private:
		static void EncodeRow(const UncompressedBitonal& uncompressed, BitStream& bs, size_t pixelY);
		static void EncodePass(BitStream& bs);
		static void EncodeVerticalMode(BitStream& bs, int a1MinusB1);
		static void EncodeHorizontalMode(BitStream& bs, int a1MinusA0, int a2MinusA1, bool white);

		static int FindNextChangedInRowOfColor(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY, bool white);
		static int FindNextChangedInRow(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY);

		static bool IsCoordinateWhite(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY);
	};
}