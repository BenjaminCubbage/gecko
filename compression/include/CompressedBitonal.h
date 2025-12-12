#pragma once
#include "BitStream.h"
#include <vector>
#include <optional>


namespace Gecko::Compression
{
	class CompressedBitonal
	{
	public:
		enum class StorageFormat { GIB /* Bitonal Data Compressed, in-house format */ };

		CompressedBitonal(BitStream bitstream, size_t width, size_t height)
			: bitstream(bitstream), width(width), height(height) {
		}

		CompressedBitonal(const CompressedBitonal& other)
			: bitstream(other.bitstream), width(other.width), height(other.height) {
		}

		CompressedBitonal(CompressedBitonal&& other) noexcept
			: bitstream(std::move(other.bitstream)), width(other.width), height(other.height) {
		}

		CompressedBitonal& operator=(const CompressedBitonal& other) = default;
		~CompressedBitonal() = default;

		BitStream& GetBitstream() { return bitstream; }
		const BitStream& GetBitstream() const { return bitstream; }
		size_t GetWidth() const { return width; }
		size_t GetHeight() const { return height; }

		static std::optional<std::vector<std::uint8_t>> TryWriteToBuffer(const CompressedBitonal& compressed, StorageFormat format);
		static std::optional<CompressedBitonal> TryReadFromBuffer(const std::vector<std::uint8_t>& buffer, StorageFormat format);


	private:
		BitStream bitstream;
		size_t width;
		size_t height;
	};
}