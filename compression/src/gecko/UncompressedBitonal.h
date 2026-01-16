#pragma once
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>


namespace Gecko::Compression
{
	class UncompressedBitonal
	{
	public:
		enum class StorageFormat { BMPStrict24 /* Bitmap file */ };

		UncompressedBitonal(std::vector<uint8_t> bgr, size_t width, size_t height)
			: bgr(bgr), width(width), height(height) { }

		UncompressedBitonal(const UncompressedBitonal& other)
			: bgr(other.bgr), width(other.width), height(other.height) { }

		UncompressedBitonal(UncompressedBitonal&& other) noexcept
			: bgr(std::move(other.bgr)), width(other.width), height(other.height) { }

		UncompressedBitonal& operator=(const UncompressedBitonal& other) = default;
		~UncompressedBitonal() = default;

		const std::vector<uint8_t>& GetBGR() const { return bgr; }
		size_t GetWidth() const { return width; }
		size_t GetHeight() const { return height; }
		bool GetPixel(size_t x, size_t y) const {
			return static_cast<uint8_t>(bgr[(y * width + x) * 3 + 0]) == 255 &&
				static_cast<uint8_t>(bgr[(y * width + x) * 3 + 1]) == 255 &&
				static_cast<uint8_t>(bgr[(y * width + x) * 3 + 2]) == 255;
		}

		static std::optional<std::vector<uint8_t>> TryWriteToBuffer(const UncompressedBitonal& uncompressed, StorageFormat format);
		static std::optional<UncompressedBitonal> TryReadFromBuffer(const std::vector<uint8_t>& buffer, StorageFormat format);

	private:
		static constexpr int HeaderSize = 54;

		std::vector<uint8_t> bgr;
		size_t width;
		size_t height;
	};
}