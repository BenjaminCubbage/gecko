#pragma once
#include "gecko/BitOperations.h"
#include "gecko/BitStream.h"
#include "gecko/CodeWords.h"
#include <algorithm>
#include <assert.h>

namespace Gecko::Compression
{
	struct Header
	{
		size_t width;
		size_t height;
	};

	/*
		Template Constraints
	*/

    template<typename HeaderWriter>
	concept IsHeaderWriter = requires (
			HeaderWriter writer,
			void* context1,
			void* context2,
			const Header& header)
	{
		bool { writer(context1, context2, header) };
	};

    template<typename ImageWriter>
    concept IsImageWriter = requires (
        	ImageWriter writer,
        	void* context1,
        	void* context2,
        	size_t pixelY,
        	size_t pixelXStart,
        	size_t pixelXEnd,
        	bool white)
    {
		bool {
			writer(
				context1,
				context2,
				pixelY,
				pixelXStart,
				pixelXEnd,
				white)
		};
    };

	/*
		Forward Declarations
	*/

	template<typename HeaderWriter>
		requires
			IsHeaderWriter<HeaderWriter>
	class DecodeHeader;

	template<typename ImageWriter>
		requires
			IsImageWriter<ImageWriter>
	class DecodeBody;

	template<typename HeaderWriter, typename ImageWriter>
		requires
			IsHeaderWriter<HeaderWriter> &&
			IsImageWriter<ImageWriter>
	class Decode
	{
	  public:
		static bool TryDecompress(
				BitStream& bs,
				void* context1 = nullptr,
				void* context2 = nullptr)
		{
			auto header = DecodeHeader<HeaderWriter>::TryRead(
				bs,
				context1,
				context2);

			return
				header &&
				DecodeBody<ImageWriter>::TryDecompress(
					bs,
					*header,
					context1,
					context2);
		}
	};

	template<typename HeaderWriter>
		requires
			IsHeaderWriter<HeaderWriter>
	class DecodeHeader
	{
  	  public:
		static std::optional<Header> TryRead(
				BitStream& bs,
				void* context1,
				void* context2)
		{
			constexpr size_t MinimumHeaderSize{ 2 + 4 + 4 + 4 };
			constexpr size_t MaximumHeaderSize{ 2 + 4 + 4 + 4 };
			size_t bytesRead{};

			/*
				Magic (BC)
			*/

			uint8_t magicB = static_cast<uint8_t>(bs.Read(8));
			uint8_t magicC = static_cast<uint8_t>(bs.Read(8));
			bytesRead += 2;

			if (magicB != 'B' || magicC != 'C')
				return std::nullopt;

			/*
				Header Size
			*/

			uint32_t headerSize = static_cast<uint8_t>(bs.Read(32));
			bytesRead += 4;

			if constexpr (std::endian::native == std::endian::big)
				headerSize = BitOperations::ByteSwap(headerSize);

			if (headerSize < MinimumHeaderSize ||
				headerSize > MaximumHeaderSize)
				return std::nullopt;

			/*
				Width / Height
			*/

			uint32_t width  = static_cast<uint8_t>(bs.Read(32));
			uint32_t height = static_cast<uint8_t>(bs.Read(32));
			bytesRead += 8;

			if constexpr (std::endian::native == std::endian::big)
			{
				width  = BitOperations::ByteSwap(width);
				height = BitOperations::ByteSwap(height);
			}

			if (width == 0 || height == 0)
				return std::nullopt;

			/* Advance to end of header */
			assert(bytesRead <= headerSize);
			bs.Advance(headerSize - bytesRead);

			Header header{
				.width  = width,
				.height = height
			};

			return HeaderWriter{}(context1, context2, header)
				? std::optional{ header }
				: std::nullopt;
		}
	};

	template<typename ImageWriter>
		requires IsImageWriter<ImageWriter>
	class DecodeBody
	{
	  private:
		struct WriterContext
		{
			void* context1;
			void* context2;
		};

        struct Row
        {
            int pixelY;
            std::vector<bool> pixels;
        };

	  public:
		static bool TryDecompress(
				BitStream& bs,
				const Header& header,
				void* context1 = nullptr,
				void* context2 = nullptr)
		{
			assert(header.width > 0 && header.height > 0);

			WriterContext writerc{
				.context1 = context1,
				.context2 = context2
			};

			std::optional<Row> row1{ Row{} };
			std::optional<Row> row2{ Row{} };

			row1->pixels.resize(header.width, true);
			row2->pixels.resize(header.width, true);

			for (size_t i = 0; i < header.height; ++i)
			{
				std::optional<Row>& curr = i % 2 ? row1 : row2;
				std::optional<Row>& prev = i % 2 ? row2 : row1;

				prev->pixelY = (int)i - 1;
				curr->pixelY = (int)i;

				if (!DecodeRow(bs, header, writerc, i != 0 ? prev : std::nullopt, *curr))
					return false;
			}

			return true;
		}

	  private:
		static bool DecodeRow(
				BitStream& bs,
				const Header& header,
				const WriterContext& writerc,
				const std::optional<Row>& previousRow,
				Row& currentRow)
		{
			int a0{ -1 };
			int a1{ -1 };
			int a2{ -1 };
			bool a0IsWhite{ true };

			while (a0 < static_cast<int>(header.width))
			{
				CodeWords::ModePrefix& prefix = CodeWords::LookupModePrefixFromLow7(bs.Peek(7));

				if (!prefix.prefixLengthBits)
					/* Prefix not found */
					return false;

				bs.Advance(prefix.prefixLengthBits);

				int b1{};
				int b2{};

				if (previousRow)
				{
					b1 = FindNextChangedInRowOfColor(bs, header, *previousRow, a0, !a0IsWhite);
					b2 = FindNextChangedInRow		(bs, header, *previousRow, b1);
				}
				else
					b1 = b2 = (int)header.width;

				switch (prefix.type)
				{
				case CodeWords::ModePrefixType::Pass:
					if (!WritePixelSpan(bs, header, writerc, currentRow, a0, b2 - 1, a0IsWhite))
						return false;
					a0 = b2;
					break;

				case CodeWords::ModePrefixType::Horizontal:
				{
					int a1Delta = 0;
					int a2Delta = 0;

					if (!DecodeHorizontalIntegrals(bs, header, a0IsWhite, &a1Delta, &a2Delta))
						/* Invalid horizontal integrals */
						return false;

					a1 = a0 + a1Delta;
					a2 = a1 + a2Delta;

					bool r{ true };
					if (a1 > a0) r = r && WritePixelSpan(bs, header, writerc, currentRow, a0, a1 - 1, a0IsWhite);
					if (a2 > a1) r = r && WritePixelSpan(bs, header, writerc, currentRow, a1, a2 - 1, !a0IsWhite);
					if (!r) return false;

					a0 = a2;
				}
				break;

				case CodeWords::ModePrefixType::Vertical0:
				case CodeWords::ModePrefixType::VerticalR1:
				case CodeWords::ModePrefixType::VerticalR2:
				case CodeWords::ModePrefixType::VerticalR3:
				case CodeWords::ModePrefixType::VerticalL1:
				case CodeWords::ModePrefixType::VerticalL2:
				case CodeWords::ModePrefixType::VerticalL3:
					a1 = b1 + CodeWords::IntegralDifferenceFromVerticalModePrefixType(prefix.type);
					a2 = a1;

					if (a1 > a0)
					{
						if (!WritePixelSpan(bs, header, writerc, currentRow, a0, a1 - 1, a0IsWhite))
							return false;
					}

					a0 = a1;
					a0IsWhite = !a0IsWhite;
					break;
				}
			}

			return true;
		}

        static bool DecodeHorizontalIntegrals(
				BitStream& bs,
				const Header& header,
                bool a0IsWhite,
                int* outA1Delta,
                int* outA2Delta)
        {
            auto getPrefix   = a0IsWhite ? &CodeWords::LookupWhiteIntegralPrefixFromLow13 : &CodeWords::LookupBlackIntegralPrefixFromLow13;
            auto getOpposite = a0IsWhite ? &CodeWords::LookupBlackIntegralPrefixFromLow13 : &CodeWords::LookupWhiteIntegralPrefixFromLow13;

            *outA1Delta = 0;
            *outA2Delta = 0;

            for (int safety = 0; safety < 100; ++safety)
            {
                auto& prefix = getPrefix(bs.Peek(13));
                if (prefix.prefixLengthBits == 0)
                    /* Bad horizontal mode prefix */
                    return false;

                bs.Advance(prefix.prefixLengthBits);
                *outA1Delta += prefix.integral;

                if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
                    break;
            }

            for (int safety = 0; safety < 100; ++safety)
            {
                auto& prefix = getOpposite(bs.Peek(13));
                if (prefix.prefixLengthBits == 0)
                    /* Bad horizontal mode prefix */
                    return false;

                bs.Advance(prefix.prefixLengthBits);
                *outA2Delta += prefix.integral;

                if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
                    break;
            }

            return true;
        }

        static int FindNextChangedInRowOfColor(
				BitStream& bs,
				const Header& header,
                const Row& row,
                int vPixelX,
                bool white)
        {
            int changedX = FindNextChangedInRow(bs, header, row, vPixelX);

            return IsPixelConsideredWhite(bs, header, row, changedX) == white
                ? changedX
                : FindNextChangedInRow(bs, header, row, changedX);
        }

        static int FindNextChangedInRow(
				BitStream& bs,
				const Header& header,
                const Row& row,
                int vPixelX)
        {
            bool originalIsWhite = IsPixelConsideredWhite(bs, header, row, vPixelX);

            for (int i = vPixelX + 1; i < (int)header.width; ++i)
                if (IsPixelConsideredWhite(bs, header, row, i) != originalIsWhite)
                    return i;

            return (int)header.width;
        }

        static bool IsPixelConsideredWhite(
				BitStream& bs,
				const Header& header,
                const Row& row,
                int vPixelX)
        {
            /* Out of bounds = white */
            return vPixelX < 0 || vPixelX >= (int)header.width || row.pixels[vPixelX];
        }

        static bool WritePixelSpan(
				BitStream& bs,
				const Header& header,
                const WriterContext& writerc,
                Row& row,
                int vPixelXStart,
                int vPixelXEnd,
                bool white)
        {
            assert(vPixelXStart <= vPixelXEnd);

            if (vPixelXEnd < 0 ||
                vPixelXStart >= (int)header.width)
                return true;

            for (int i = vPixelXStart; i <= vPixelXEnd; ++i)
            {
                if (i < 0) continue;
                if (i >= (int)header.width) break;

                row.pixels[i] = white;
            }

            return ImageWriter{}(
                writerc.context1,
                writerc.context2,
                row.pixelY,
                std::clamp(vPixelXStart, 0, (int)header.width - 1),
                std::clamp(vPixelXEnd,   0, (int)header.width - 1),
                white);
        }
	};
}