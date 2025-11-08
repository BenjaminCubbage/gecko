#pragma once
#include <vector>
#include <optional>
#include <stdexcept>

class UncompressedBitonal
{
public:
	enum class StorageFormat { BMPStrict24 /* Bitmap file */ };

	UncompressedBitonal(std::vector<std::byte> bgr, size_t width, size_t height) 
		: bgr(bgr), width(width), height(height) 
	{
		if (bgr.size() != width * height * 3)
			throw std::invalid_argument("Malformed BGR buffer: bgr.size() != width * height * 3");
	}

	UncompressedBitonal(const UncompressedBitonal& other)
		: bgr(other.bgr), width(other.width), height(other.height) { }

	UncompressedBitonal(UncompressedBitonal&& other) noexcept
		: bgr(std::move(other.bgr)), width(other.width), height(other.height) { }

	UncompressedBitonal& operator=(const UncompressedBitonal& other) = default;
	~UncompressedBitonal() = default;

	const std::vector<std::byte>& GetBGR() const { return bgr; }
	size_t GetWidth() const { return width; }
	size_t GetHeight() const { return height; }
	bool GetPixel(size_t x, size_t y) const { return static_cast<uint8_t>(bgr[(y * width + x) * 3 + 0]) == 255 &&
											         static_cast<uint8_t>(bgr[(y * width + x) * 3 + 1]) == 255 &&
											         static_cast<uint8_t>(bgr[(y * width + x) * 3 + 2]) == 255; }

	static std::optional<std::vector<std::byte>> TryWriteToBuffer(const UncompressedBitonal& uncompressed, StorageFormat format);
	static std::optional<UncompressedBitonal> TryReadFromBuffer(const std::vector<std::byte>& buffer, StorageFormat format);

private:
	static constexpr int HeaderSize = 54;

	std::vector<std::byte> bgr;
	size_t width;
	size_t height;
};