#pragma once
#include "gecko/CompressedBitonal.h"
#include "gecko/UncompressedBitonal.h"
#include "gecko/BitStream.h"

namespace Gecko::Compression
{
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