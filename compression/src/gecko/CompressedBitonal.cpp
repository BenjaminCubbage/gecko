#include "gecko/CompressedBitonal.h"
#include "gecko/BitStream.h"


namespace Gecko::Compression
{
	std::optional<std::vector<uint8_t>> CompressedBitonal::TryWriteToBuffer(const CompressedBitonal& compressed, StorageFormat format)
	{
		static constexpr uint32_t HeaderSize = 14;

		if (format != StorageFormat::GIB)
			throw std::invalid_argument("The specified format is not currently supported. `GIB` is the only "
				"supported export format for `GIB`.");

		auto bytes = compressed.bitstream.GetBytes();

		if (bytes.empty())
			return std::nullopt;

		std::vector<uint8_t> buffer(bytes.size() + HeaderSize);

		const uint32_t w = static_cast<uint32_t>(compressed.width);
		const uint32_t h = static_cast<uint32_t>(compressed.height);

		// Signature
		buffer[0] = uint8_t{ 'B' };
		buffer[1] = uint8_t{ 'C' };

		// Header Size
		buffer[2] = static_cast<uint8_t>(HeaderSize);
		buffer[3] = static_cast<uint8_t>(HeaderSize >> 8);
		buffer[4] = static_cast<uint8_t>(HeaderSize >> 16);
		buffer[5] = static_cast<uint8_t>(HeaderSize >> 24);

		// Width
		buffer[6] = static_cast<uint8_t>(w);
		buffer[7] = static_cast<uint8_t>(w >> 8);
		buffer[8] = static_cast<uint8_t>(w >> 16);
		buffer[9] = static_cast<uint8_t>(w >> 24);

		// Height
		buffer[10] = static_cast<uint8_t>(h);
		buffer[11] = static_cast<uint8_t>(h >> 8);
		buffer[12] = static_cast<uint8_t>(h >> 16);
		buffer[13] = static_cast<uint8_t>(h >> 24);

		// Data
		std::copy(bytes.begin(), bytes.end(), buffer.data() + HeaderSize);
		return buffer;
	}


	std::optional<CompressedBitonal> CompressedBitonal::TryReadFromBuffer(const std::vector<uint8_t>& buffer, StorageFormat format)
	{
		static constexpr uint32_t HeaderSize = 14;

		if (format != StorageFormat::GIB || buffer.size() < HeaderSize + 1)
			return std::nullopt;

		if (static_cast<char>(buffer[0]) != 'B' || static_cast<char>(buffer[1]) != 'C')
			return std::nullopt;

		auto read32 = [&](size_t offset) -> uint32_t {
			return (static_cast<uint32_t>(buffer[offset + 0])) |
				   (static_cast<uint32_t>(buffer[offset + 1]) << 8) |
				   (static_cast<uint32_t>(buffer[offset + 2]) << 16) |
				   (static_cast<uint32_t>(buffer[offset + 3]) << 24);
			};

		const uint32_t headerSize = read32(2);
		if (headerSize != HeaderSize)
			return std::nullopt;

		const uint32_t w = read32(6);
		const uint32_t h = read32(10);

		if (w == 0 || h == 0)
			return std::nullopt;

		std::vector<uint8_t> bytes(buffer.size() - HeaderSize);
		std::copy(buffer.begin() + HeaderSize, buffer.end(), bytes.begin());

		return CompressedBitonal{ BitStream{ std::move(bytes) }, w, h };
	}
}