#pragma once
#include "gecko/BitOperations.h"
#include "gecko/BitStream.h"
#include "gecko/CodeWords.h"
#include <bit>
#include <limits>
#include <unordered_map>

namespace Gecko::Compression
{
    /*
        Template Constraints
    */

	template<typename ImageReader>
	concept IsImageReader = requires (
			ImageReader reader,
			void* context1,
			void* context2,
			size_t posX,
			size_t posY)
	{
		bool{ reader(context1, context2, posX, posY) };
	};
    
    /*
        Foward Declarations
    */

    template<typename ImageReader>
	class EncodeHeader;
    
	template<typename ImageReader>
		requires
			IsImageReader<ImageReader>
	class EncodeBody;

	template<typename ImageReader>
		requires
			IsImageReader<ImageReader>
	class Encode
	{
	  public:
		static std::optional<BitStream> TryCompress(
				size_t width,
				size_t height,
				void* context1 = nullptr,
				void* context2 = nullptr)
		{
            BitStream bs;

            if (!EncodeHeader<ImageReader>::TryWrite(bs, width, height) ||
                !EncodeBody<ImageReader>::TryCompress(
                    bs,
                    width,
                    height,
                    context1,
                    context2))
                return std::nullopt;

            return std::move(bs);
		}
	};

    template<typename ImageReader>
	class EncodeHeader
	{
      public:
        static bool TryWrite(
                BitStream& bs,
                size_t width,
                size_t height)
        {
            static constexpr size_t HeaderSize{ 2 + 4 + 4 + 4 };

            if (width  > (std::numeric_limits<uint32_t>::max)() ||
                height > (std::numeric_limits<uint32_t>::max)())
                return false;

            bs.PushBack(static_cast<uint32_t>('B'), 8);
            bs.PushBack(static_cast<uint32_t>('C'), 8);

            uint32_t writeHeaderSize = static_cast<uint32_t>(HeaderSize);
            uint32_t writeWidth      = static_cast<uint32_t>(width);
            uint32_t writeHeight     = static_cast<uint32_t>(height);

            if constexpr (std::endian::native == std::endian::big)
            {
                writeHeaderSize = BitOperations::ByteSwap(writeHeaderSize);
                writeWidth      = BitOperations::ByteSwap(writeWidth);
                writeHeight     = BitOperations::ByteSwap(writeHeight);
            }

            bs.PushBack(writeHeaderSize, 32);
            bs.PushBack(writeWidth,      32);
            bs.PushBack(writeHeight,     32);
            return true;
        }
	};

	template<typename ImageReader>
		requires
			IsImageReader<ImageReader>
	class EncodeBody
	{
	  private:
		struct ImageReaderContext
		{
			size_t width;
			size_t height;
			void* context1;
			void* context2;
		};

	  public:
		static bool TryCompress(
                BitStream& bs,
                size_t width,
				size_t height,
				void* context1 = nullptr,
				void* context2 = nullptr)
		{
            assert(width > 0 && height > 0);

			ImageReaderContext readerc{
				.width    = width,
				.height   = height,
				.context1 = context1,
				.context2 = context2
			};

			for (size_t i = 0; i < height; ++i)
				EncodeRow(readerc, bs, i);

            return true;
		}

	  private:
		static void EncodeRow(
                const ImageReaderContext& readerc,
                BitStream& bs,
                size_t pixelY)
		{
			int a0 = -1;
			int a1 = FindNextChangedInRow(readerc, a0, static_cast<int>(pixelY));
			int a2 = FindNextChangedInRow(readerc, a1, static_cast<int>(pixelY));
			bool a0IsWhite = IsCoordinateWhite(readerc, a0, static_cast<int>(pixelY));

			int b1 = FindNextChangedInRowOfColor(readerc, a0, static_cast<int>(pixelY - 1), !a0IsWhite);
			int b2 = FindNextChangedInRow(readerc, b1, static_cast<int>(pixelY - 1));

			while (a0 < static_cast<signed>(readerc.width))
			{
				if (b2 < a1)
				{
					EncodePass(bs);
					a0 = b2;
				}
				else if (std::abs(a1 - b1) <= 3)
				{
					EncodeVerticalMode(bs, a1 - b1);
					a0 = a1;
				}
				else
				{
					EncodeHorizontalMode(bs, a1 - a0, a2 - a1, a0IsWhite);
					a0 = a2;
				}

				a1 = FindNextChangedInRow(readerc, a0, static_cast<int>(pixelY));
				a2 = FindNextChangedInRow(readerc, a1, static_cast<int>(pixelY));
				a0IsWhite = IsCoordinateWhite(readerc, a0, static_cast<int>(pixelY));

				b1 = FindNextChangedInRowOfColor(readerc, a0, static_cast<int>(pixelY - 1), !a0IsWhite);
				b2 = FindNextChangedInRow(readerc, b1, static_cast<int>(pixelY - 1));
			}
		}

		static void EncodePass(BitStream& bs)
		{
			const auto& prefix = CodeWords::LookupModePrefixFromType(CodeWords::ModePrefixType::Pass);
			bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);
		}

		static void EncodeVerticalMode(BitStream& bs, int a1MinusB1)
		{
            assert(a1MinusB1 >= -3 && a1MinusB1 <= 3);

			static const std::unordered_map<int, CodeWords::ModePrefixType> map = {
				{-1,  CodeWords::ModePrefixType::VerticalL1 },
				{-2,  CodeWords::ModePrefixType::VerticalL2 },
				{-3,  CodeWords::ModePrefixType::VerticalL3 },
				{ 0,  CodeWords::ModePrefixType::Vertical0  },
				{ 1,  CodeWords::ModePrefixType::VerticalR1 },
				{ 2,  CodeWords::ModePrefixType::VerticalR2 },
				{ 3,  CodeWords::ModePrefixType::VerticalR3 }
			};

			const auto& prefix = CodeWords::LookupModePrefixFromType(map.at(a1MinusB1));
			bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);
		}

		static void EncodeHorizontalMode(
                BitStream& bs,
                int a1MinusA0,
                int a2MinusA1,
                bool a0IsWhite)
		{
			bs.PushBack(0b001, 3);
			auto getPrefix   = a0IsWhite ? &CodeWords::GetNextWhiteIntegralPrefix : &CodeWords::GetNextBlackIntegralPrefix;
			auto getOpposite = a0IsWhite ? &CodeWords::GetNextBlackIntegralPrefix : &CodeWords::GetNextWhiteIntegralPrefix;

			for (int remaining = a1MinusA0;;)
			{
				const auto& prefix = getPrefix(remaining);

				remaining -= prefix.integral;
				bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);

				if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
					break;
			}

			for (int remaining = a2MinusA1;;)
			{
				const auto& prefix = getOpposite(remaining);

				remaining -= prefix.integral;
				bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);

				if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
					break;
			}
		}

		static int FindNextChangedInRowOfColor(
                const ImageReaderContext& readerc,
                int vPixelX,
                int vPixelY,
                bool white)
		{
			int changedX = FindNextChangedInRow(readerc, vPixelX, vPixelY);

			return IsCoordinateWhite(readerc, changedX, vPixelY) == white
				? changedX
				: FindNextChangedInRow(readerc, changedX, vPixelY);
		}

		static int FindNextChangedInRow(
                const ImageReaderContext& readerc,
                int vPixelX,
                int vPixelY)
		{
			bool originalIsWhite = IsCoordinateWhite(readerc, vPixelX, vPixelY);

			for (int i = vPixelX; i < static_cast<int>(readerc.width); ++i)
				if (IsCoordinateWhite(readerc, i, vPixelY) != originalIsWhite)
					return i;

			/* We can return a value outside bounds because we're returning a virtual coordinate. */
			return static_cast<int>(readerc.width);
		}

		static bool IsCoordinateWhite(
                const ImageReaderContext& readerc,
                int vPixelX,
                int vPixelY)
		{
			if (vPixelX < 0 || static_cast<size_t>(std::max(vPixelX, 0)) >= readerc.width ||
				vPixelY < 0 || static_cast<size_t>(std::max(vPixelY, 0)) >= readerc.height)
				return true;

			return ImageReader{}(readerc.context1, readerc.context2, vPixelX, vPixelY);
		}
	};
}