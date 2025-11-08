#include "UncompressedBitonal.h"
#include <span>
#include <stdexcept>
#include <bit>

#include "BitOperations.h"

std::optional<std::vector<std::byte>> UncompressedBitonal::TryWriteToBuffer(const UncompressedBitonal& uncompressed, StorageFormat format)
{
	if (format != StorageFormat::BMPStrict24)
		throw std::invalid_argument("The specified format is not currently supported. `BMPStrict24` is the only "
									"supported export format for `UncompressedBitonal`.");

	if (uncompressed.width <= 0 || uncompressed.height <= 0)
		return std::nullopt;

	const uint32_t rowBytesLen = (uncompressed.width * 3 + 3) & ~3;
	std::vector<std::byte> buffer(HeaderSize + (rowBytesLen * uncompressed.height));
	std::span<std::byte> headerBytes(buffer.data(), HeaderSize);
	std::span<std::byte> pixelsBytes(buffer.data() + HeaderSize, buffer.size() - HeaderSize);

	if (uncompressed.bgr.size() != uncompressed.width * uncompressed.height * 3)
		return std::nullopt;

	auto w32 = [&](size_t offset, uint32_t v) {
		headerBytes[offset + 0] = std::byte(v & 0xFF);
		headerBytes[offset + 1] = std::byte((v >> 8) & 0xFF);
		headerBytes[offset + 2] = std::byte((v >> 16) & 0xFF);
		headerBytes[offset + 3] = std::byte((v >> 24) & 0xFF);
	};

	auto w16 = [&](size_t offset, uint16_t v) {
		headerBytes[offset + 0] = std::byte(v & 0xFF);
		headerBytes[offset + 1] = std::byte((v >> 8) & 0xFF);
	};

	buffer[0] = std::byte('B');
	buffer[1] = std::byte('M');
	w32(2, static_cast<uint32_t>(buffer.size()));
	w32(10, HeaderSize);
	w32(14, 40);
	w32(18, static_cast<uint32_t>(uncompressed.width));
	w32(22, static_cast<uint32_t>(uncompressed.height));
	w16(26, 1);
	w16(28, 24);
	w32(34, rowBytesLen * static_cast<uint32_t>(uncompressed.height));

	for (size_t row = 0; row < uncompressed.height; ++row)
	{
		std::span<std::byte> rowBytes(pixelsBytes.data() + (row * rowBytesLen), rowBytesLen);
		memcpy(rowBytes.data(), uncompressed.bgr.data() + (row * uncompressed.width * 3), uncompressed.width * 3);
	}

	return buffer;
}

std::optional<UncompressedBitonal> UncompressedBitonal::TryReadFromBuffer(const std::vector<std::byte>& buffer, StorageFormat format)
{
	if (format != StorageFormat::BMPStrict24 || buffer.size() < HeaderSize + 3)
		return std::nullopt;

	std::span<const std::byte> headerBytes{ buffer.data(), HeaderSize };
	std::span<const std::byte> pixelsBytes{ buffer.data() + HeaderSize, buffer.size() - HeaderSize };

	if (static_cast<char>(headerBytes[0]) != 'B' || static_cast<char>(headerBytes[1]) != 'M')
		return std::nullopt;

	uint32_t headerSize = 0;
	memcpy(&headerSize, headerBytes.data() + 14, 4);
	if constexpr (std::endian::native == std::endian::big)
		headerSize = BitOperations::SwapEndianness32(headerSize);
	if (headerSize != 40) return std::nullopt;


	uint32_t w{ 0 };
	uint32_t h{ 0 };
	memcpy(&w, headerBytes.data() + 18, 4);
	memcpy(&h, headerBytes.data() + 22, 4);

	if constexpr (std::endian::native == std::endian::big)
	{
		w = BitOperations::SwapEndianness32(w);
		h = BitOperations::SwapEndianness32(h);
	}


	uint16_t bpp = 0;
	memcpy(&bpp, headerBytes.data() + 28, 2);
	if constexpr (std::endian::native == std::endian::big)
		bpp = BitOperations::SwapEndianness16(bpp);
	if (bpp != 24) return std::nullopt;


	// Row widths are end-padded to 4 bytes
	int rowBytesLen = (w * 3 + 3) & ~3;
	if (static_cast<size_t>(rowBytesLen) * h > pixelsBytes.size())
		return std::nullopt;


	std::vector<std::byte> bgr(w * h * 3);

	for (uint32_t row = 0; row < h; ++row)
	for (uint32_t col = 0; col < w; ++col)
	{
		bgr[(row * w + col) * 3 + 0] = pixelsBytes[row * rowBytesLen + col * 3 + 0];
		bgr[(row * w + col) * 3 + 1] = pixelsBytes[row * rowBytesLen + col * 3 + 1];
		bgr[(row * w + col) * 3 + 2] = pixelsBytes[row * rowBytesLen + col * 3 + 2];
	}
	
	return UncompressedBitonal{ std::move(bgr), w, h };
}