#pragma once
#include <vector>
#include <span>
#include <bitset>
#include <stdexcept>
#include <iostream>
#include "BitOperations.h"

class BitStream
{
private:
	struct BitPosition
	{
		size_t byte = 0;
		size_t bit = 0;
	};

	std::vector<std::byte> bytes;

	/*
	*		`readerPos` and `writerPos` are used to track the current location of the reader/writer. 
	*	Currently, `writerPos` is analogous to a stream EOF.
	*		
	*		`readerPos` is allowed to extend past `writerPos`, in which case all extra bits read will
	*	be zero.
	* 
	*		Because `writerPos` is analogous to EOF, it should never be moved backwards. If this
	*	functionality is needed in the future, a seperate EOF member field should be created to
	*	keep track of the stream's EOF location. For now, this system is good enough for our
	*	purposes.
	*/
	BitPosition readerPos;
	BitPosition writerPos;

	void ProgressReaderPos(size_t by) { readerPos.bit += by; readerPos.byte += readerPos.bit / 8; readerPos.bit %= 8; }
	void ProgressWriterPos(size_t by) { writerPos.bit += by; writerPos.byte += writerPos.bit / 8; writerPos.bit %= 8; }

	/*
	*		If we don't have an extra six zeroed bytes at the end of our `bytes` vector, we can't read
	*	all zeroes in the event the `readerPos` extends beyond the `writerPos`. `writerPos` also
	*	depends on this when writing--it should always be ensured there is enough end-padding to
	*	write to without extending beyond the bounds of the vector.
	* 
	*		Call this whenever `writerPos` changes--on construction and `PushBack()`. We don't call 
	*	this on `Peek()` because it's a `const` function. I also don't call this in
	*	`ProgressWriterPos` for decoupling and complexity reasons, but this might be a mistake.
	*/
	void EnsureAdequateEOFWiggleRoom()
	{
		/*
		 *		Add +24 instead of +6 so we don't need to resize the vector on every small write.
		 *	This may be space-inefficient with many `BitStream`s, but for now I'm prioritizing
		 *	speed over space efficiency.
		*/
		if (bytes.size() < writerPos.byte + 6)
			bytes.resize(bytes.size() + 24);
	}

public:
	BitStream(std::vector<std::byte> bytes)
		: bytes(bytes), readerPos(BitPosition{ .byte = 0, .bit = 0 }), writerPos(BitPosition{ .byte = bytes.size(), .bit = 0 }) 
	{
		EnsureAdequateEOFWiggleRoom();
	}

	BitStream() : BitStream(std::vector<std::byte>{ }) {};

	BitStream(const BitStream& other)
		: bytes(other.bytes), readerPos(other.readerPos), writerPos(other.writerPos) {}

	BitStream(BitStream&& other) noexcept
		: bytes(std::move(other.bytes)), readerPos(other.readerPos), writerPos(other.writerPos) {}

	BitStream& operator=(const BitStream& other) = default;
	~BitStream() = default;


	void SeekStart();
	void StepForward(size_t by);
	uint32_t Peek(size_t numBits) const;
	void PushBack(uint32_t value, size_t numBits);
	std::span<const std::byte> Bytes() const;
};