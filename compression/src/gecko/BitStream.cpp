#include "gecko/BitStream.h"
#include <algorithm>
#include <bit>
#include <cstring>

namespace Gecko::Compression
{
	void BitStream::SeekStart()
	{
		readerPos.byte = 0;
		readerPos.bit = 0;
	}

	void BitStream::StepForward(size_t by)
	{
		ProgressReaderPos(by);
	}

	uint32_t BitStream::Peek(size_t numBits) const
	{
		if (numBits <= 0 || numBits > 32)
			throw std::invalid_argument("Expected `numbits` to be (0, 32].");

		//	If this ever throws, I've made a serious error. See `PushBack()`.
		if (bytes.size() < writerPos.byte + 6)
			throw std::out_of_range("Internal writer position (EOF) did not have at least 6 bytes of "
				"wiggle room for zeroed reading.");

		/*
		*		`clampedReaderPos` makes sure we don't read past the vector. Since `Peek()` reads 5 bytes, and the
		*	last 5 bytes of `bytes` is specified to be all zeroes, we can clamp `clampedReaderPos.byte` to be
		*	a maximum of `writerPos.byte + 1`
		*/
		BitPosition clampedReaderPos
		{
			.byte = std::min(readerPos.byte, writerPos.byte + 1),
			.bit = readerPos.bit
		};

		uint32_t hi = 0;
		uint8_t  lo = 0;
		memcpy(&hi, bytes.data() + clampedReaderPos.byte, sizeof(hi));
		memcpy(&lo, bytes.data() + clampedReaderPos.byte + 4, sizeof(lo));

		if constexpr (std::endian::native == std::endian::little)
			hi = BitOperations::SwapEndianness32(hi);

		hi <<= clampedReaderPos.bit;
		hi >>= 32 - numBits;

		/*
		*		Annoyingly, bitshifting by `>= 8` on `uint8_t` or equivalent is UB, so we have to do this
		*	hack instead.
		*/
		lo >>= std::min(40 - clampedReaderPos.bit - numBits, static_cast<size_t>(7));
		lo >>= static_cast<unsigned int>((40 - clampedReaderPos.bit - numBits) >= 8);

		return hi | lo;
	}

	void BitStream::PushBack(uint32_t value, size_t numBits)
	{
		/*
		*	This should never throw because we're calling `EnsureAdequateEOFWiggleRoom()` at
		* the end of this function. If it throws, `writerPos` is being changed somewhere
		* and the byte array isn't being resized for adequate right-padded zeroes.
		*/
		if (bytes.size() < writerPos.byte + 6)
			throw std::out_of_range("Internal writer position (EOF) did not have at least 6 bytes of "
				"wiggle room for zeroed reading.");

		uint32_t hi = value;
		uint8_t  lo = value;

		hi <<= 32 - numBits;
		hi >>= writerPos.bit;

		int bitsLost = std::max(static_cast<signed>(writerPos.bit) - (32 - static_cast<signed>(numBits)), 0);
		int shiftAmt = std::clamp(8 - bitsLost, 0, 8);

		lo <<= std::min(shiftAmt, 7);
		lo <<= static_cast<uint8_t>(shiftAmt == 8);

		if constexpr (std::endian::native == std::endian::little)
			hi = BitOperations::SwapEndianness32(hi);

		uint32_t oldHi = 0;
		memcpy(&oldHi, &bytes[writerPos.byte], 4);

		hi |= oldHi;
		memcpy(&bytes[writerPos.byte], &hi, 4);
		memcpy(&bytes[writerPos.byte + 4], &lo, 1);

		ProgressWriterPos(numBits);
		EnsureAdequateEOFWiggleRoom();
	}

	std::span<const uint8_t> BitStream::GetBytes() const
	{
		size_t spanSize = writerPos.byte + (writerPos.bit == 0 ? 0 : 1);

		if (spanSize > bytes.size())
			throw std::out_of_range("Internal `writerPos` (EOF) was beyond the bounds of the byte buffer.");

		return std::span<const uint8_t>(bytes.data(), spanSize);
	}
}