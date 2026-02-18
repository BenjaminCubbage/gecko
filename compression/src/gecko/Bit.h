#pragma once
#include <cstdint>

namespace Gecko::Compression
{

class Bit
{
    public:
        /*
                Polyfills for std::byteswap
        */

        static uint32_t ByteSwap(uint32_t value)
        {
                return ((value << 24) & 0xFF000000) |
                        ((value << 8)  & 0x00FF0000) |
                        ((value >> 8)  & 0x0000FF00) |
                        ((value >> 24) & 0x000000FF);
        }

        static uint16_t ByteSwap(uint16_t value)
        {
                return (value << 8) | (value >> 8);
        }
}; /* class Bit */

} /* namespace Gecko::Compression */