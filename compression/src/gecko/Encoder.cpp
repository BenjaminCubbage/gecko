#include "gecko/Encoder.h"
#include "gecko/CodeWords.h"
#include <unordered_map>


namespace Gecko::Compression
{
	std::optional<CompressedBitonal> Encoder::TryCompressBitonal(const UncompressedBitonal& uncompressed)
	{
		if (!uncompressed.GetWidth() || !uncompressed.GetHeight())
			return std::nullopt;

		BitStream bs;

		for (size_t i = 0; i < uncompressed.GetHeight(); ++i)
			EncodeRow(uncompressed, bs, i);

		return CompressedBitonal{ std::move(bs), uncompressed.GetWidth(), uncompressed.GetHeight() };
	}


	void Encoder::EncodeRow(const UncompressedBitonal& uncompressed, BitStream& bs, size_t pixelY)
	{
		int a0 = -1;
		int a1 = FindNextChangedInRow(uncompressed, a0, static_cast<int>(pixelY));
		int a2 = FindNextChangedInRow(uncompressed, a1, static_cast<int>(pixelY));
		bool a0IsWhite = Encoder::IsCoordinateWhite(uncompressed, a0, static_cast<int>(pixelY));

		int b1 = FindNextChangedInRowOfColor(uncompressed, a0, static_cast<int>(pixelY - 1), !a0IsWhite);
		int b2 = FindNextChangedInRow(uncompressed, b1, static_cast<int>(pixelY - 1));

		while (a0 < static_cast<signed>(uncompressed.GetWidth()))
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

			a1 = FindNextChangedInRow(uncompressed, a0, static_cast<int>(pixelY));
			a2 = FindNextChangedInRow(uncompressed, a1, static_cast<int>(pixelY));
			a0IsWhite = Encoder::IsCoordinateWhite(uncompressed, a0, static_cast<int>(pixelY));

			b1 = FindNextChangedInRowOfColor(uncompressed, a0, static_cast<int>(pixelY - 1), !a0IsWhite);
			b2 = FindNextChangedInRow(uncompressed, b1, static_cast<int>(pixelY - 1));
		}
	}


	void Encoder::EncodePass(BitStream& bs)
	{
		const auto& prefix = CodeWords::LookupModePrefixFromType(CodeWords::ModePrefixType::Pass);
		bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);
	}


	void Encoder::EncodeVerticalMode(BitStream& bs, int a1MinusB1)
	{
		static const std::unordered_map<int, CodeWords::ModePrefixType> map = {
			{ 0,  CodeWords::ModePrefixType::Vertical0  },
			{ 1,  CodeWords::ModePrefixType::VerticalR1 },
			{ 2,  CodeWords::ModePrefixType::VerticalR2 },
			{ 3,  CodeWords::ModePrefixType::VerticalR3 },
			{-1,  CodeWords::ModePrefixType::VerticalL1 },
			{-2,  CodeWords::ModePrefixType::VerticalL2 },
			{-3,  CodeWords::ModePrefixType::VerticalL3 }
		};

		const auto it = map.find(a1MinusB1);

		if (it == map.end())
			throw std::invalid_argument("a1MinusB1 must be between [-3, 3] for vertical encoding. "
				"Otherwise, horizontal encoding must be used.");

		const auto& prefix = CodeWords::LookupModePrefixFromType(it->second);
		bs.PushBack(prefix.prefixBits, prefix.prefixLengthBits);
	}


	void Encoder::EncodeHorizontalMode(BitStream& bs, int a1MinusA0, int a2MinusA1, bool a0IsWhite)
	{
		bs.PushBack(0b001, 3);
		auto getPrefix = a0IsWhite ? &CodeWords::GetNextWhiteIntegralPrefix : &CodeWords::GetNextBlackIntegralPrefix;
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


	int Encoder::FindNextChangedInRowOfColor(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY, bool white)
	{
		int changedX = FindNextChangedInRow(uncompressed, vPixelX, vPixelY);

		return IsCoordinateWhite(uncompressed, changedX, vPixelY) == white
			? changedX
			: FindNextChangedInRow(uncompressed, changedX, vPixelY);
	}


	int Encoder::FindNextChangedInRow(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY)
	{
		bool originalIsWhite = IsCoordinateWhite(uncompressed, vPixelX, vPixelY);

		for (int i = vPixelX; i < static_cast<int>(uncompressed.GetWidth()); ++i)
			if (IsCoordinateWhite(uncompressed, i, vPixelY) != originalIsWhite)
				return i;

		/* We can return a value outside bounds because we're returning a virtual coordinate. */
		return static_cast<int>(uncompressed.GetWidth());
	}


	bool Encoder::IsCoordinateWhite(const UncompressedBitonal& uncompressed, int vPixelX, int vPixelY)
	{
		if (vPixelX < 0 || static_cast<size_t>(std::max(vPixelX, 0)) >= uncompressed.GetWidth() ||
			vPixelY < 0 || static_cast<size_t>(std::max(vPixelY, 0)) >= uncompressed.GetHeight())
			return true;

		return uncompressed.GetPixel(vPixelX, vPixelY);
	}
}