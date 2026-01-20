#pragma once
#include <cstdint>

namespace Gecko::Compression
{
	class BitOperations
	{
	  public:
		static uint32_t SwapEndianness32(uint32_t value);
		static uint16_t SwapEndianness16(uint16_t value);
	};
}