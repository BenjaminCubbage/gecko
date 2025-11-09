#pragma once
#include "CompressedBitonal.h"
#include "UncompressedBitonal.h"
#include "BitStream.h"


namespace Gecko::Compression
{
	class Decoder
	{
	public:
		enum class Result { Success, Aborted };
		static std::optional<UncompressedBitonal> TryDecompressBitonal(CompressedBitonal& compressed);

	private:
		static bool DecodeRow(CompressedBitonal& compressed, std::vector<std::byte>& bgr, size_t pixelY);
		static bool DecodeHorizontalIntegrals(CompressedBitonal& compressed, std::vector<std::byte>& bgr, bool a0IsWhite, int* outA1Delta, int* outA2Delta);

		static int FindNextChangedInRowOfColor(CompressedBitonal& compressed, std::vector<std::byte>& bgr, int vPixelX, int vPixelY, bool white);
		static int FindNextChangedInRow(CompressedBitonal& compressed, std::vector<std::byte>& bgr, int vPixelX, int vPixelY);

		static bool IsCoordinateWhite(CompressedBitonal& compressed, std::vector<std::byte>& bgr, int vPixelX, int vPixelY);
		static void WriteRowPixelsRange(CompressedBitonal& compressed, std::vector<std::byte>& bgr, int vPixelXStart, int vPixelXEnd, int pixelY, bool white);
	};
}