#include "gecko/Decoder.h"
#include "gecko/CodeWords.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

namespace Gecko::Compression
{
	bool Decoder::TryDecompressBitonal(CompressedBitonal compressed, const Drawer& drawer)
	{
		compressed.GetBitstream().SeekStart();

		if (compressed.GetWidth() <= 0 || compressed.GetHeight() <= 0)
			return false;

		std::optional<Row> row1{ Row{} };
		std::optional<Row> row2{ Row{} };

		row1->pixels.resize(compressed.GetWidth(), true);
		row2->pixels.resize(compressed.GetWidth(), true);

		for (size_t i = 0; i < compressed.GetHeight(); ++i)
		{
			std::optional<Row>& curr = i % 2 ? row1 : row2;
			std::optional<Row>& prev = i % 2 ? row2 : row1;

			prev->pixelY = (int)i - 1;
			curr->pixelY = (int)i;

			if (!DecodeRow(compressed, i != 0 ? prev : std::nullopt, *curr, drawer))
				return false;
		}

		return true;
	}

	bool Decoder::DecodeRow(CompressedBitonal& compressed,
							const std::optional<Row>& previousRow,
							Row& currentRow,
							const Drawer& drawer)
	{
		int a0{ -1 };
		int a1{ -1 };
		int a2{ -1 };
		bool a0IsWhite{ true };

		while (a0 < static_cast<int>(compressed.GetWidth()))
		{
			CodeWords::ModePrefix& prefix = CodeWords::LookupModePrefixFromLow7(compressed.GetBitstream().Peek(7));

			if (!prefix.prefixLengthBits)
				/* Prefix not found */
				return false;

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);

			int b1{};
			int b2{};

			if (previousRow)
			{
				b1 = FindNextChangedInRowOfColor(compressed, *previousRow, a0, !a0IsWhite);
				b2 = FindNextChangedInRow		(compressed, *previousRow, b1);
			}
			else
				b1 = b2 = (int)compressed.GetWidth();

			switch (prefix.type)
			{
			case CodeWords::ModePrefixType::Pass:
				WritePixelSpan(compressed, currentRow, a0, b2 - 1, a0IsWhite, drawer);
				a0 = b2;
				break;

			case CodeWords::ModePrefixType::Horizontal:
			{
				int a1Delta = 0;
				int a2Delta = 0;

				if (!DecodeHorizontalIntegrals(compressed, a0IsWhite, &a1Delta, &a2Delta))
					/* Invalid horizontal integrals */
					return false;

				a1 = a0 + a1Delta;
				a2 = a1 + a2Delta;

				if (a1 > a0) WritePixelSpan(compressed, currentRow, a0, a1 - 1, a0IsWhite,  drawer);
				if (a2 > a1) WritePixelSpan(compressed, currentRow, a1, a2 - 1, !a0IsWhite, drawer);

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
					WritePixelSpan(compressed, currentRow, a0, a1 - 1, a0IsWhite, drawer);

				a0 = a1;
				a0IsWhite = !a0IsWhite;
				break;
			}
		}

		return true;
	}

	bool Decoder::DecodeHorizontalIntegrals(CompressedBitonal& compressed, bool a0IsWhite, int* outA1Delta, int* outA2Delta)
	{
		auto getPrefix   = a0IsWhite ? &CodeWords::LookupWhiteIntegralPrefixFromLow13 : &CodeWords::LookupBlackIntegralPrefixFromLow13;
		auto getOpposite = a0IsWhite ? &CodeWords::LookupBlackIntegralPrefixFromLow13 : &CodeWords::LookupWhiteIntegralPrefixFromLow13;

		*outA1Delta = 0;
		*outA2Delta = 0;

		for (int safety = 0; safety < 100; ++safety)
		{
			auto& prefix = getPrefix(compressed.GetBitstream().Peek(13));
			if (prefix.prefixLengthBits == 0)
				/* Bad horizontal mode prefix */
				return false;

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);
			*outA1Delta += prefix.integral;

			if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
				break;
		}

		for (int safety = 0; safety < 100; ++safety)
		{
			auto& prefix = getOpposite(compressed.GetBitstream().Peek(13));
			if (prefix.prefixLengthBits == 0)
				/* Bad horizontal mode prefix */
				return false;

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);
			*outA2Delta += prefix.integral;

			if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
				break;
		}

		return true;
	}

	int Decoder::FindNextChangedInRowOfColor(CompressedBitonal& compressed, const Row& row, int vPixelX, bool white)
	{
		int changedX = FindNextChangedInRow(compressed, row, vPixelX);

		return Decoder::IsPixelConsideredWhite(compressed, row, changedX) == white
			? changedX
			: FindNextChangedInRow(compressed, row, changedX);
	}

	int Decoder::FindNextChangedInRow(CompressedBitonal& compressed, const Row& row, int vPixelX)
	{
		bool originalIsWhite = IsPixelConsideredWhite(compressed, row, vPixelX);

		for (int i = vPixelX + 1; i < (int)compressed.GetWidth(); ++i)
			if (IsPixelConsideredWhite(compressed, row, i) != originalIsWhite)
				return i;

		return (int)compressed.GetWidth();
	}

	bool Decoder::IsPixelConsideredWhite(CompressedBitonal& compressed, const Row& row, int vPixelX)
	{
		/* Out of bounds = white */
		return vPixelX < 0 || vPixelX >= (int)compressed.GetWidth() || row.pixels[vPixelX];
	}

	void Decoder::WritePixelSpan(CompressedBitonal& compressed,
						         Row& row,
							     int vPixelXStart,
							     int vPixelXEnd,
							     bool white,
							     const Drawer& drawer)
	{
		assert(vPixelXStart <= vPixelXEnd);

		if (vPixelXEnd < 0 ||
			vPixelXStart >= (int)compressed.GetWidth())
			return;

		for (int i = vPixelXStart; i <= vPixelXEnd; ++i)
		{
			if (i < 0) continue;
			if (i >= (int)compressed.GetWidth()) break;

			row.pixels[i] = white;
		}

		drawer.handler(
			drawer.context1,
			drawer.context2,
			row.pixelY,
			std::clamp(vPixelXStart, 0, (int)compressed.GetWidth() - 1),
			std::clamp(vPixelXEnd,   0, (int)compressed.GetWidth() - 1),
			white);
	}
}