#pragma once
#include <algorithm>
#include <assert.h>
#include <vector>
#include "Bit.h"

namespace Gecko::Compression
{

class BitStream
{
    private:
        struct BitPosition
        {
                size_t byte{};
                size_t bit{};
        };

    public:
        BitStream(std::vector<uint8_t> bytes)
                : bytes{ std::move(bytes) }
                , readerPos{ BitPosition{ .byte = 0,            .bit = 0 } }
                , writerPos{ BitPosition{ .byte = bytes.size(), .bit = 0 } }
        {
                EnsureAdequateEOFWiggleRoom();
        }

        BitStream()
                : BitStream{std::vector<uint8_t>{} } {}

        BitStream           (const BitStream& other) = delete;
        BitStream& operator=(const BitStream& other) = delete;
        BitStream           (BitStream&& other) noexcept = default;
        BitStream& operator=(BitStream&& other) noexcept = default;

        ~BitStream() = default;

        /*
                Move reader back to start.
        */
        void SeekStart()
        {
                readerPos.byte = 0;
                readerPos.bit  = 0;
        }

        /*
                Read the next n bits.

                The result is "right-aligned" so that the LSB stores the
                last bit read.
        */
        uint32_t Read(size_t numBits)
        {
                const uint32_t result = Peek(numBits);
                ProgressReaderPos(numBits);
                return result;
        }

        /*
                Peak the next n bits.

                The result is "right-aligned" so that the LSB stores the
                last bit read.
        */
        uint32_t Peek(size_t numBits) const
        {
                assert(numBits > 0 && numBits <= 32);
                assert(bytes.size() >= writerPos.byte + 6);

                /*
                        Make sure we don't read past the vector. Since Peek() reads
                        5 bytes, and the last 5 bytes beyond the writer is always
                        zeroes, we can clamp `clampedReaderPos.byte` to be a maximum
                        of `writerPos.byte + 1`
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
                        hi = Bit::ByteSwap(hi);

                hi <<= clampedReaderPos.bit;
                hi >>= 32 - numBits;

                lo >>= std::min(40 - clampedReaderPos.bit - numBits, static_cast<size_t>(7));
                lo >>= static_cast<unsigned int>((40 - clampedReaderPos.bit - numBits) >= 8);

                return hi | lo;
        }

        /*
                Advance reader by n bits.
        */
        void Advance(size_t by)
        {
                ProgressReaderPos(by);
        }

        /*
                Write the least significants of value to the bit stream.
        */
        void PushBack(uint32_t value, size_t numBits)
        {
                assert(bytes.size() >= writerPos.byte + 6);

                uint32_t hi = value;
                uint8_t  lo = value;

                hi <<= 32 - numBits;
                hi >>= writerPos.bit;

                int bitsLost = std::max(static_cast<signed>(writerPos.bit) - (32 - static_cast<signed>(numBits)), 0);
                int shiftAmt = std::clamp(8 - bitsLost, 0, 8);

                lo <<= std::min(shiftAmt, 7);
                lo <<= static_cast<uint8_t>(shiftAmt == 8);

                if constexpr (std::endian::native == std::endian::little)
                        hi = Bit::ByteSwap(hi);

                uint32_t oldHi = 0;
                memcpy(&oldHi, &bytes[writerPos.byte], 4);

                hi |= oldHi;
                memcpy(&bytes[writerPos.byte],     &hi, 4);
                memcpy(&bytes[writerPos.byte + 4], &lo, 1);

                ProgressWriterPos(numBits);
                EnsureAdequateEOFWiggleRoom();
        }

        /*
                Move the underlying buffer out of the BitStream, invalidating
                this object in the process.
        */
        std::vector<uint8_t> Release()
        {
                bytes.resize(writerPos.bit == 0
                        ? writerPos.byte
                        : writerPos.byte + 1);

                return std::move(bytes);
        }

    private:
        void ProgressReaderPos(size_t by) { readerPos.bit += by; readerPos.byte += readerPos.bit / 8; readerPos.bit %= 8; }
        void ProgressWriterPos(size_t by) { writerPos.bit += by; writerPos.byte += writerPos.bit / 8; writerPos.bit %= 8; }

        /*
                We need enough spare bytes at the end to read "all zeroes"
                when the reader has fully surpassed the writer.

                Call this after moving the writer.
        */
        void EnsureAdequateEOFWiggleRoom()
        {
                /*
                        Make sure the last five bytes of the array are all zeroes.
                */
                if (bytes.size() < writerPos.byte + 6)
                        bytes.resize(bytes.size() + 24);
        }

        /*
                readerPos can extend beyond writerPos, where all extra bits
                should be read as zero.

                writerPos is treated as EOF here, so it shouldn't move
                backwards.
        */
        BitPosition readerPos;
        BitPosition writerPos;

        std::vector<uint8_t> bytes;
}; /* class BitStream */

} /* Gecko::Compression */