#include "BitOperations.h"

uint32_t BitOperations::SwapEndianness32(uint32_t value)
{
    return ((value << 24) & 0xFF000000) |
           ((value << 8)  & 0x00FF0000) |
           ((value >> 8)  & 0x0000FF00) |
           ((value >> 24) & 0x000000FF);
}

uint16_t BitOperations::SwapEndianness16(uint16_t value)
{
    return (value << 8) | (value >> 8);
}
