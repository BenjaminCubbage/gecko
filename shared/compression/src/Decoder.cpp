#include "Decoder.h"
#include <iostream>
#include <algorithm>
#include "CodeWords.h"


namespace Gecko::Compression
{
	std::optional<UncompressedBitonal> Decoder::TryDecompressBitonal(CompressedBitonal& compressed)
	{
		compressed.GetBitstream().SeekStart();

		if (!compressed.GetWidth() || !compressed.GetHeight())
			return std::nullopt;

		std::vector<uint8_t> bgr(compressed.GetWidth() * compressed.GetHeight() * 3);

		for (size_t i = 0; i < compressed.GetHeight(); ++i)
		{
			if (!DecodeRow(compressed, bgr, i))
				return std::nullopt;
		}

		return UncompressedBitonal{ std::move(bgr), compressed.GetWidth(), compressed.GetHeight() };
	}


	bool Decoder::DecodeRow(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, size_t pixelY)
	{
		int a0 = -1;
		int a1 = -1;
		int a2 = -1;
		bool a0IsWhite = true;

		while (a0 < static_cast<int>(compressed.GetWidth()))
		{
			CodeWords::ModePrefix& prefix = CodeWords::LookupModePrefixFromLow7(compressed.GetBitstream().Peek(7));

			if (!prefix.prefixLengthBits)
				return false; // Prefix not found

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);

			int b1 = FindNextChangedInRowOfColor(compressed, bgr, a0, static_cast<int>(pixelY - 1), !a0IsWhite);
			int b2 = FindNextChangedInRow(compressed, bgr, b1, static_cast<int>(pixelY - 1));


			switch (prefix.type)
			{
			case CodeWords::ModePrefixType::Pass:
				WriteRowPixelsRange(compressed, bgr, a0, b2 - 1, static_cast<int>(pixelY), a0IsWhite);
				a0 = b2;
				break;

			case CodeWords::ModePrefixType::Horizontal:
			{
				int a1Delta = 0;
				int a2Delta = 0;

				if (!DecodeHorizontalIntegrals(compressed, bgr, a0IsWhite, &a1Delta, &a2Delta))
					return false; // invalid horizontal integrals

				a1 = a0 + a1Delta;
				a2 = a1 + a2Delta;

				if (a1 > a0) WriteRowPixelsRange(compressed, bgr, a0, a1 - 1, static_cast<int>(pixelY), a0IsWhite);
				if (a2 > a1) WriteRowPixelsRange(compressed, bgr, a1, a2 - 1, static_cast<int>(pixelY), !a0IsWhite);

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
					WriteRowPixelsRange(compressed, bgr, a0, a1 - 1, static_cast<int>(pixelY), a0IsWhite);

				a0 = a1;
				a0IsWhite = !a0IsWhite;
				break;
			}
		}

		return true;
	}


	bool Decoder::DecodeHorizontalIntegrals(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, bool a0IsWhite, int* outA1Delta, int* outA2Delta)
	{
		auto getPrefix = a0IsWhite ? &CodeWords::LookupWhiteIntegralPrefixFromLow13 : &CodeWords::LookupBlackIntegralPrefixFromLow13;
		auto getOpposite = a0IsWhite ? &CodeWords::LookupBlackIntegralPrefixFromLow13 : &CodeWords::LookupWhiteIntegralPrefixFromLow13;

		*outA1Delta = 0;
		*outA2Delta = 0;

		for (int safety = 0; safety < 100; ++safety)
		{
			auto& prefix = getPrefix(compressed.GetBitstream().Peek(13));
			if (prefix.prefixLengthBits == 0)
				return false; // Invalid horizontal mode prefix

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);
			*outA1Delta += prefix.integral;

			if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
				break;
		}

		for (int safety = 0; safety < 100; ++safety)
		{
			auto& prefix = getOpposite(compressed.GetBitstream().Peek(13));
			if (prefix.prefixLengthBits == 0)
				return false; // Invalid horizontal mode prefix

			compressed.GetBitstream().StepForward(prefix.prefixLengthBits);
			*outA2Delta += prefix.integral;

			if (prefix.type == CodeWords::IntegralPrefixType::Terminating)
				break;
		}

		return true;
	}


	int Decoder::FindNextChangedInRowOfColor(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, int vPixelX, int vPixelY, bool white)
	{
		int changedX = FindNextChangedInRow(compressed, bgr, vPixelX, vPixelY);

		return Decoder::IsCoordinateWhite(compressed, bgr, changedX, vPixelY) == white
			? changedX
			: FindNextChangedInRow(compressed, bgr, changedX, vPixelY);
	}


	int Decoder::FindNextChangedInRow(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, int vPixelX, int vPixelY)
	{
		bool originalIsWhite = IsCoordinateWhite(compressed, bgr, vPixelX, vPixelY);

		for (int i = vPixelX + 1; i < static_cast<int>(compressed.GetWidth()); ++i)
			if (IsCoordinateWhite(compressed, bgr, i, vPixelY) != originalIsWhite)
				return i;

		return static_cast<int>(compressed.GetWidth());
	}


	bool Decoder::IsCoordinateWhite(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, int vPixelX, int vPixelY)
	{
		if (vPixelX < 0 || vPixelX >= static_cast<int>(compressed.GetWidth()) ||
			vPixelY < 0 || vPixelY >= static_cast<int>(compressed.GetHeight()))
			return true; // out of bounds = white

		const size_t idx = (static_cast<size_t>(vPixelY) * compressed.GetWidth() + vPixelX) * 3;

		const uint8_t b = static_cast<uint8_t>(bgr[idx + 0]);
		const uint8_t g = static_cast<uint8_t>(bgr[idx + 1]);
		const uint8_t r = static_cast<uint8_t>(bgr[idx + 2]);

		return b == 255 && g == 255 && r == 255;
	}


	void Decoder::WriteRowPixelsRange(CompressedBitonal& compressed, std::vector<uint8_t>& bgr, int vPixelXStart, int vPixelXEnd, int pixelY, bool white)
	{
		if (vPixelXStart > vPixelXEnd)
			throw std::invalid_argument("vPixelXStart should be less than or equal to vPixelXEnd");

		if (pixelY < 0 || static_cast<size_t>(pixelY) >= compressed.GetHeight())
			throw std::invalid_argument("pixelY should be [0, compressed.GetHeight())");

		uint8_t value = white ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0);
		for (int i = vPixelXStart; i <= vPixelXEnd; ++i)
		{
			if (i < 0 || static_cast<size_t>(std::max(i, 0)) >= compressed.GetWidth())
				continue;

			const size_t idx = (static_cast<size_t>(pixelY) * compressed.GetWidth() + static_cast<size_t>(i)) * 3;
			bgr[idx + 0] = value;
			bgr[idx + 1] = value;
			bgr[idx + 2] = value;
		}
	}
}