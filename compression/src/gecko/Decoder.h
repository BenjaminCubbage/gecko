#pragma once
#include "CompressedBitonal.h"
#include "UncompressedBitonal.h"
#include "BitStream.h"

namespace Gecko::Compression
{
	class Decoder
	{
	  public:
		using DrawerCallback = void (*)(void* context1,
                                        void* context2,
                                        size_t pixelY,
							 	        size_t pixelXStart,
							 	        size_t pixelXEnd,
								        bool white);

        struct Drawer
        {
            DrawerCallback handler;

            void* context1;
            void* context2;
        };

        struct Row
        {
            int pixelY;
            std::vector<bool> pixels;
        };

		enum class Result { OK, Aborted };
		static bool TryDecompressBitonal(CompressedBitonal compressed, const Drawer& drawer);

	  private:
		static bool DecodeRow(CompressedBitonal& compressed, const std::optional<Row>& previousRow, Row& currentRow, const Drawer& drawer);
		static bool DecodeHorizontalIntegrals(CompressedBitonal& compressed, bool a0IsWhite, int* outA1Delta, int* outA2Delta);

		static int FindNextChangedInRowOfColor(CompressedBitonal& compressed, const Row& row, int vPixelX, bool white);
		static int FindNextChangedInRow(CompressedBitonal& compressed, const Row& row, int vPixelX);

		static bool IsPixelConsideredWhite(CompressedBitonal& compressed, const Row& row, int vPixelX);
		
		static void WritePixelSpan(CompressedBitonal& compressed,
					     		   Row& row,
					     		   int vPixelXStart,
					     		   int vPixelYEnd,
					     		   bool white,
					     		   const Drawer& drawer);
	};
}