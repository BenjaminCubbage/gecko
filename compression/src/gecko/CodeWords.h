#pragma once
#include <algorithm>
#include <array>
#include <assert.h>
#include <bit>
#include <cstdint>
#include <iostream>
#include <optional>

/*
        See: https://www.itu.int/rec/T-REC-T.6/en
*/

namespace Gecko::Compression
{

class IntegralPrefixTable
{
    public:
        struct IntegralPrefix
        {
                uint8_t  live;
                uint16_t value;
                uint8_t  numBits;
                uint8_t  makeup;
                uint8_t  continuationIndex;
        };

        struct IntegralCode
        {
                uint16_t bits;
                uint16_t numBits;
                uint16_t value;
                bool     makeup;
        };

        const IntegralPrefix& DecodeBlack13(size_t bits13) const
        {
                assert(m_first8Black.size() == (1u << 8));

                bits13 &= (1u << 13) - 1;

                size_t hi8 = bits13 >> 5;
                size_t lo5 = bits13 & ((1u << 5) - 1);

                auto& first = m_first8Black[hi8];

                if (!first.live || first.numBits)
                        return first;

                assert(first.continuationIndex < m_final5Black.size());
                assert(m_final5Black[first.continuationIndex].size() == (1u << 5));
                return m_final5Black[first.continuationIndex][lo5];
        }

        const IntegralPrefix& DecodeWhite13(size_t bits13) const
        {
                assert(m_first8White.size() == (1u << 8));

                bits13 &= (1u << 13) - 1;

                size_t hi8 = bits13 >> 5;
                size_t lo5 = bits13 & ((1u << 5) - 1);

                auto& first = m_first8White[hi8];

                if (!first.live || first.numBits)
                        return first;

                assert(first.continuationIndex < m_final5White.size());
                assert(m_final5White[first.continuationIndex].size() == (1u << 5));
                return m_final5White[first.continuationIndex][lo5];
        }

        static const IntegralCode& EncodeBlack(size_t value)
        {
                static constexpr std::array<IntegralCode, 64> terminating =
                {
                        IntegralCode{ 0b0000110111,   10 },
                        IntegralCode{ 0b010,           3 },
                        IntegralCode{ 0b11,            2 },
                        IntegralCode{ 0b10,            2 },
                        IntegralCode{ 0b011,           3 },
                        IntegralCode{ 0b0011,          4 },
                        IntegralCode{ 0b0010,          4 },
                        IntegralCode{ 0b00011,         5 },
                        IntegralCode{ 0b000101,        6 },
                        IntegralCode{ 0b000100,        6 },
                        IntegralCode{ 0b0000100,       7 },
                        IntegralCode{ 0b0000101,       7 },
                        IntegralCode{ 0b0000111,       7 },
                        IntegralCode{ 0b00000100,      8 },
                        IntegralCode{ 0b00000111,      8 },
                        IntegralCode{ 0b000011000,     9 },
                        IntegralCode{ 0b0000010111,   10 },
                        IntegralCode{ 0b0000011000,   10 },
                        IntegralCode{ 0b0000001000,   10 },
                        IntegralCode{ 0b00001100111,  11 },
                        IntegralCode{ 0b00001101000,  11 },
                        IntegralCode{ 0b00001101100,  11 },
                        IntegralCode{ 0b00000110111,  11 },
                        IntegralCode{ 0b00000101000,  11 },
                        IntegralCode{ 0b00000010111,  11 },
                        IntegralCode{ 0b00000011000,  11 },
                        IntegralCode{ 0b000011001010, 12 },
                        IntegralCode{ 0b000011001011, 12 },
                        IntegralCode{ 0b000011001100, 12 },
                        IntegralCode{ 0b000011001101, 12 },
                        IntegralCode{ 0b000001101000, 12 },
                        IntegralCode{ 0b000001101001, 12 },
                        IntegralCode{ 0b000001101010, 12 },
                        IntegralCode{ 0b000001101011, 12 },
                        IntegralCode{ 0b000011010010, 12 },
                        IntegralCode{ 0b000011010011, 12 },
                        IntegralCode{ 0b000011010100, 12 },
                        IntegralCode{ 0b000011010101, 12 },
                        IntegralCode{ 0b000011010110, 12 },
                        IntegralCode{ 0b000011010111, 12 },
                        IntegralCode{ 0b000001101100, 12 },
                        IntegralCode{ 0b000001101101, 12 },
                        IntegralCode{ 0b000011011010, 12 },
                        IntegralCode{ 0b000011011011, 12 },
                        IntegralCode{ 0b000001010100, 12 },
                        IntegralCode{ 0b000001010101, 12 },
                        IntegralCode{ 0b000001010110, 12 },
                        IntegralCode{ 0b000001010111, 12 },
                        IntegralCode{ 0b000001100100, 12 },
                        IntegralCode{ 0b000001100101, 12 },
                        IntegralCode{ 0b000001010010, 12 },
                        IntegralCode{ 0b000001010011, 12 },
                        IntegralCode{ 0b000000100100, 12 },
                        IntegralCode{ 0b000000110111, 12 },
                        IntegralCode{ 0b000000111000, 12 },
                        IntegralCode{ 0b000000100111, 12 },
                        IntegralCode{ 0b000000101000, 12 },
                        IntegralCode{ 0b000001011000, 12 },
                        IntegralCode{ 0b000001011001, 12 },
                        IntegralCode{ 0b000000101011, 12 },
                        IntegralCode{ 0b000000101100, 12 },
                        IntegralCode{ 0b000001011010, 12 },
                        IntegralCode{ 0b000001100110, 12 },
                        IntegralCode{ 0b000001100111, 12 }
                };

                static constexpr std::array<IntegralCode, 40> makeup =
                {
                        IntegralCode{ 0b0000001111,    10,   64, 1 },
                        IntegralCode{ 0b000011001000,  12,  128, 1 },
                        IntegralCode{ 0b000011001001,  12,  192, 1 },
                        IntegralCode{ 0b000001011011,  12,  256, 1 },
                        IntegralCode{ 0b000000110011,  12,  320, 1 },
                        IntegralCode{ 0b000000110100,  12,  384, 1 },
                        IntegralCode{ 0b000000110101,  12,  448, 1 },
                        IntegralCode{ 0b0000001101100, 13,  512, 1 },
                        IntegralCode{ 0b0000001101101, 13,  576, 1 },
                        IntegralCode{ 0b0000001001010, 13,  640, 1 },
                        IntegralCode{ 0b0000001001011, 13,  704, 1 },
                        IntegralCode{ 0b0000001001100, 13,  768, 1 },
                        IntegralCode{ 0b0000001001101, 13,  832, 1 },
                        IntegralCode{ 0b0000001110010, 13,  896, 1 },
                        IntegralCode{ 0b0000001110011, 13,  960, 1 },
                        IntegralCode{ 0b0000001110100, 13, 1024, 1 },
                        IntegralCode{ 0b0000001110101, 13, 1088, 1 },
                        IntegralCode{ 0b0000001110110, 13, 1152, 1 },
                        IntegralCode{ 0b0000001110111, 13, 1216, 1 },
                        IntegralCode{ 0b0000001010010, 13, 1280, 1 },
                        IntegralCode{ 0b0000001010011, 13, 1344, 1 },
                        IntegralCode{ 0b0000001010100, 13, 1408, 1 },
                        IntegralCode{ 0b0000001010101, 13, 1472, 1 },
                        IntegralCode{ 0b0000001011010, 13, 1536, 1 },
                        IntegralCode{ 0b0000001011011, 13, 1600, 1 },
                        IntegralCode{ 0b0000001100100, 13, 1664, 1 },
                        IntegralCode{ 0b0000001100101, 13, 1728, 1 },
                        IntegralCode{ 0b00000001000,   11, 1792, 1 },
                        IntegralCode{ 0b00000001100,   11, 1856, 1 },
                        IntegralCode{ 0b00000001101,   11, 1920, 1 },
                        IntegralCode{ 0b000000010010,  12, 1984, 1 },
                        IntegralCode{ 0b000000010011,  12, 2048, 1 },
                        IntegralCode{ 0b000000010100,  12, 2112, 1 },
                        IntegralCode{ 0b000000010101,  12, 2176, 1 },
                        IntegralCode{ 0b000000010110,  12, 2240, 1 },
                        IntegralCode{ 0b000000010111,  12, 2304, 1 },
                        IntegralCode{ 0b000000011100,  12, 2368, 1 },
                        IntegralCode{ 0b000000011101,  12, 2432, 1 },
                        IntegralCode{ 0b000000011110,  12, 2496, 1 },
                        IntegralCode{ 0b000000011111,  12, 2560, 1 }
                };

                if (value < 64)
                        return terminating[value];

                size_t idx = (std::min)(value, size_t{ 2560 }) / 64 - 1;
                assert(idx < makeup.size());

                return makeup[idx];
        }

        static const IntegralCode& EncodeWhite(size_t value)
        {
                static constexpr std::array<IntegralCode, 64> terminating =
                {
                        IntegralCode{ 0b00110101, 8 },
                        IntegralCode{ 0b000111,   6 },
                        IntegralCode{ 0b0111,     4 },
                        IntegralCode{ 0b1000,     4 },
                        IntegralCode{ 0b1011,     4 },
                        IntegralCode{ 0b1100,     4 },
                        IntegralCode{ 0b1110,     4 },
                        IntegralCode{ 0b1111,     4 },
                        IntegralCode{ 0b10011,    5 },
                        IntegralCode{ 0b10100,    5 },
                        IntegralCode{ 0b00111,    5 },
                        IntegralCode{ 0b01000,    5 },
                        IntegralCode{ 0b001000,   6 },
                        IntegralCode{ 0b000011,   6 },
                        IntegralCode{ 0b110100,   6 },
                        IntegralCode{ 0b110101,   6 },
                        IntegralCode{ 0b101010,   6 },
                        IntegralCode{ 0b101011,   6 },
                        IntegralCode{ 0b0100111,  7 },
                        IntegralCode{ 0b0001100,  7 },
                        IntegralCode{ 0b0001000,  7 },
                        IntegralCode{ 0b0010111,  7 },
                        IntegralCode{ 0b0000011,  7 },
                        IntegralCode{ 0b0000100,  7 },
                        IntegralCode{ 0b0101000,  7 },
                        IntegralCode{ 0b0101011,  7 },
                        IntegralCode{ 0b0010011,  7 },
                        IntegralCode{ 0b0100100,  7 },
                        IntegralCode{ 0b0011000,  7 },
                        IntegralCode{ 0b00000010, 8 },
                        IntegralCode{ 0b00000011, 8 },
                        IntegralCode{ 0b00011010, 8 },
                        IntegralCode{ 0b00011011, 8 },
                        IntegralCode{ 0b00010010, 8 },
                        IntegralCode{ 0b00010011, 8 },
                        IntegralCode{ 0b00010100, 8 },
                        IntegralCode{ 0b00010101, 8 },
                        IntegralCode{ 0b00010110, 8 },
                        IntegralCode{ 0b00010111, 8 },
                        IntegralCode{ 0b00101000, 8 },
                        IntegralCode{ 0b00101001, 8 },
                        IntegralCode{ 0b00101010, 8 },
                        IntegralCode{ 0b00101011, 8 },
                        IntegralCode{ 0b00101100, 8 },
                        IntegralCode{ 0b00101101, 8 },
                        IntegralCode{ 0b00000100, 8 },
                        IntegralCode{ 0b00000101, 8 },
                        IntegralCode{ 0b00001010, 8 },
                        IntegralCode{ 0b00001011, 8 },
                        IntegralCode{ 0b01010010, 8 },
                        IntegralCode{ 0b01010011, 8 },
                        IntegralCode{ 0b01010100, 8 },
                        IntegralCode{ 0b01010101, 8 },
                        IntegralCode{ 0b00100100, 8 },
                        IntegralCode{ 0b00100101, 8 },
                        IntegralCode{ 0b01011000, 8 },
                        IntegralCode{ 0b01011001, 8 },
                        IntegralCode{ 0b01011010, 8 },
                        IntegralCode{ 0b01011011, 8 },
                        IntegralCode{ 0b01001010, 8 },
                        IntegralCode{ 0b01001011, 8 },
                        IntegralCode{ 0b00110010, 8 },
                        IntegralCode{ 0b00110011, 8 },
                        IntegralCode{ 0b00110100, 8 }
                };

                static constexpr std::array<IntegralCode, 40> makeup =
                {
                        IntegralCode{ 0b11011,         5,   64, 1 },
                        IntegralCode{ 0b10010,         5,  128, 1 },
                        IntegralCode{ 0b010111,        6,  192, 1 },
                        IntegralCode{ 0b0110111,       7,  256, 1 },
                        IntegralCode{ 0b00110110,      8,  320, 1 },
                        IntegralCode{ 0b00110111,      8,  384, 1 },
                        IntegralCode{ 0b01100100,      8,  448, 1 },
                        IntegralCode{ 0b01100101,      8,  512, 1 },
                        IntegralCode{ 0b01101000,      8,  576, 1 },
                        IntegralCode{ 0b01100111,      8,  640, 1 },
                        IntegralCode{ 0b011001100,     9,  704, 1 },
                        IntegralCode{ 0b011001101,     9,  768, 1 },
                        IntegralCode{ 0b011010010,     9,  832, 1 },
                        IntegralCode{ 0b011010011,     9,  896, 1 },
                        IntegralCode{ 0b011010100,     9,  960, 1 },
                        IntegralCode{ 0b011010101,     9, 1024, 1 },
                        IntegralCode{ 0b011010110,     9, 1088, 1 },
                        IntegralCode{ 0b011010111,     9, 1152, 1 },
                        IntegralCode{ 0b011011000,     9, 1216, 1 },
                        IntegralCode{ 0b011011001,     9, 1280, 1 },
                        IntegralCode{ 0b011011010,     9, 1344, 1 },
                        IntegralCode{ 0b011011011,     9, 1408, 1 },
                        IntegralCode{ 0b010011000,     9, 1472, 1 },
                        IntegralCode{ 0b010011001,     9, 1536, 1 },
                        IntegralCode{ 0b010011010,     9, 1600, 1 },
                        IntegralCode{ 0b011000,        6, 1664, 1 },
                        IntegralCode{ 0b010011011,     9, 1728, 1 },
                        IntegralCode{ 0b00000001000,  11, 1792, 1 },
                        IntegralCode{ 0b00000001100,  11, 1856, 1 },
                        IntegralCode{ 0b00000001101,  11, 1920, 1 },
                        IntegralCode{ 0b000000010010, 12, 1984, 1 },
                        IntegralCode{ 0b000000010011, 12, 2048, 1 },
                        IntegralCode{ 0b000000010100, 12, 2112, 1 },
                        IntegralCode{ 0b000000010101, 12, 2176, 1 },
                        IntegralCode{ 0b000000010110, 12, 2240, 1 },
                        IntegralCode{ 0b000000010111, 12, 2304, 1 },
                        IntegralCode{ 0b000000011100, 12, 2368, 1 },
                        IntegralCode{ 0b000000011101, 12, 2432, 1 },
                        IntegralCode{ 0b000000011110, 12, 2496, 1 },
                        IntegralCode{ 0b000000011111, 12, 2560, 1 }
                };

                if (value < 64)
                        return terminating[value];

                size_t idx = (std::min)(value, size_t{ 2560 }) / 64 - 1;
                assert(idx < makeup.size());

                return makeup[idx];
        }

        IntegralPrefixTable()
        {
                /*
                        Black
                */

                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b11000] = { 1, 0, 10, 0, 0 };

                m_first8Black[0b01000000] = { 1,  1, 3, 0, 0 };
                m_first8Black[0b11000000] = { 1,  2, 2, 0, 0 };
                m_first8Black[0b10000000] = { 1,  3, 2, 0, 0 };
                m_first8Black[0b01100000] = { 1,  4, 3, 0, 0 };
                m_first8Black[0b00110000] = { 1,  5, 4, 0, 0 };
                m_first8Black[0b00100000] = { 1,  6, 4, 0, 0 };
                m_first8Black[0b00011000] = { 1,  7, 5, 0, 0 };
                m_first8Black[0b00010100] = { 1,  8, 6, 0, 0 };
                m_first8Black[0b00010000] = { 1,  9, 6, 0, 0 };
                m_first8Black[0b00001000] = { 1, 10, 7, 0, 0 };
                m_first8Black[0b00001010] = { 1, 11, 7, 0, 0 };
                m_first8Black[0b00001110] = { 1, 12, 7, 0, 0 };
                m_first8Black[0b00000100] = { 1, 13, 8, 0, 0 };
                m_first8Black[0b00000111] = { 1, 14, 8, 0, 0 };

                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b00000] = { 1, 15,  9, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b11000] = { 1, 16, 10, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b00000] = { 1, 17, 10, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b00000] = { 1, 18, 10, 0, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b11100] = { 1, 19, 11, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b00000] = { 1, 20, 11, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b10000] = { 1, 21, 11, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b11100] = { 1, 22, 11, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b00000] = { 1, 23, 11, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b11100] = { 1, 24, 11, 0, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b00000] = { 1, 25, 11, 0, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b10100] = { 1, 26, 12, 0, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b10110] = { 1, 27, 12, 0, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b11000] = { 1, 28, 12, 0, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b11010] = { 1, 29, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b10000] = { 1, 30, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b10010] = { 1, 31, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b10100] = { 1, 32, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b10110] = { 1, 33, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b00100] = { 1, 34, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b00110] = { 1, 35, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b01000] = { 1, 36, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b01010] = { 1, 37, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b01100] = { 1, 38, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b01110] = { 1, 39, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b11000] = { 1, 40, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b11010] = { 1, 41, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b10100] = { 1, 42, 12, 0, 0 };
                m_first8Black[0b00001101] = { 1, 0, 0, 0, 0 }; m_final5Black[0][0b10110] = { 1, 43, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b01000] = { 1, 44, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b01010] = { 1, 45, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b01100] = { 1, 46, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b01110] = { 1, 47, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b01000] = { 1, 48, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b01010] = { 1, 49, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b00100] = { 1, 50, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b00110] = { 1, 51, 12, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01000] = { 1, 52, 12, 0, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b01110] = { 1, 53, 12, 0, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10000] = { 1, 54, 12, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01110] = { 1, 55, 12, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10000] = { 1, 56, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b10000] = { 1, 57, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b10010] = { 1, 58, 12, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10110] = { 1, 59, 12, 0, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b11000] = { 1, 60, 12, 0, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b10100] = { 1, 61, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b01100] = { 1, 62, 12, 0, 0 };
                m_first8Black[0b00000110] = { 1, 0, 0, 0, 3 }; m_final5Black[3][0b01110] = { 1, 63, 12, 0, 0 };

                // 0000001111001
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b11000] = { 1,   64, 10, 1, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b10000] = { 1,  128, 12, 1, 0 };
                m_first8Black[0b00001100] = { 1, 0, 0, 0, 1 }; m_final5Black[1][0b10010] = { 1,  192, 12, 1, 0 };
                m_first8Black[0b00000101] = { 1, 0, 0, 0, 2 }; m_final5Black[2][0b10110] = { 1,  256, 12, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b00110] = { 1,  320, 12, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b01000] = { 1,  384, 12, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b01010] = { 1,  448, 12, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b01100] = { 1,  512, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b01101] = { 1,  576, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01010] = { 1,  640, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01011] = { 1,  704, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01100] = { 1,  768, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b01101] = { 1,  832, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10010] = { 1,  896, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10011] = { 1,  960, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10100] = { 1, 1024, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10101] = { 1, 1088, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10110] = { 1, 1152, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b10111] = { 1, 1216, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10010] = { 1, 1280, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10011] = { 1, 1344, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10100] = { 1, 1408, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b10101] = { 1, 1472, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b11010] = { 1, 1536, 13, 1, 0 };
                m_first8Black[0b00000010] = { 1, 0, 0, 0, 4 }; m_final5Black[4][0b11011] = { 1, 1600, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b00100] = { 1, 1664, 13, 1, 0 };
                m_first8Black[0b00000011] = { 1, 0, 0, 0, 5 }; m_final5Black[5][0b00101] = { 1, 1728, 13, 1, 0 };

                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b00000] = { 1, 1792, 11, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b10000] = { 1, 1856, 11, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b10100] = { 1, 1920, 11, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b00100] = { 1, 1984, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b00110] = { 1, 2048, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b01000] = { 1, 2112, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b01010] = { 1, 2176, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b01100] = { 1, 2240, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b01110] = { 1, 2304, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b11000] = { 1, 2368, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b11010] = { 1, 2432, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b11100] = { 1, 2496, 12, 1, 0 };
                m_first8Black[0b00000001] = { 1, 0, 0, 0, 6 }; m_final5Black[6][0b11110] = { 1, 2560, 12, 1, 0 };

                /*
                        White
                */

                m_first8White[0b00110101] = { 1,  0, 8, 0, 0 };
                m_first8White[0b00011100] = { 1,  1, 6, 0, 0 };
                m_first8White[0b01110000] = { 1,  2, 4, 0, 0 };
                m_first8White[0b10000000] = { 1,  3, 4, 0, 0 };
                m_first8White[0b10110000] = { 1,  4, 4, 0, 0 };
                m_first8White[0b11000000] = { 1,  5, 4, 0, 0 };
                m_first8White[0b11100000] = { 1,  6, 4, 0, 0 };
                m_first8White[0b11110000] = { 1,  7, 4, 0, 0 };
                m_first8White[0b10011000] = { 1,  8, 5, 0, 0 };
                m_first8White[0b10100000] = { 1,  9, 5, 0, 0 };
                m_first8White[0b00111000] = { 1, 10, 5, 0, 0 };
                m_first8White[0b01000000] = { 1, 11, 5, 0, 0 };
                m_first8White[0b00100000] = { 1, 12, 6, 0, 0 };
                m_first8White[0b00001100] = { 1, 13, 6, 0, 0 };
                m_first8White[0b11010000] = { 1, 14, 6, 0, 0 };
                m_first8White[0b11010100] = { 1, 15, 6, 0, 0 };
                m_first8White[0b10101000] = { 1, 16, 6, 0, 0 };
                m_first8White[0b10101100] = { 1, 17, 6, 0, 0 };
                m_first8White[0b01001110] = { 1, 18, 7, 0, 0 };
                m_first8White[0b00011000] = { 1, 19, 7, 0, 0 };
                m_first8White[0b00010000] = { 1, 20, 7, 0, 0 };
                m_first8White[0b00101110] = { 1, 21, 7, 0, 0 };
                m_first8White[0b00000110] = { 1, 22, 7, 0, 0 };
                m_first8White[0b00001000] = { 1, 23, 7, 0, 0 };
                m_first8White[0b01010000] = { 1, 24, 7, 0, 0 };
                m_first8White[0b01010110] = { 1, 25, 7, 0, 0 };
                m_first8White[0b00100110] = { 1, 26, 7, 0, 0 };
                m_first8White[0b01001000] = { 1, 27, 7, 0, 0 };
                m_first8White[0b00110000] = { 1, 28, 7, 0, 0 };
                m_first8White[0b00000010] = { 1, 29, 8, 0, 0 };
                m_first8White[0b00000011] = { 1, 30, 8, 0, 0 };
                m_first8White[0b00011010] = { 1, 31, 8, 0, 0 };
                m_first8White[0b00011011] = { 1, 32, 8, 0, 0 };
                m_first8White[0b00010010] = { 1, 33, 8, 0, 0 };
                m_first8White[0b00010011] = { 1, 34, 8, 0, 0 };
                m_first8White[0b00010100] = { 1, 35, 8, 0, 0 };
                m_first8White[0b00010101] = { 1, 36, 8, 0, 0 };
                m_first8White[0b00010110] = { 1, 37, 8, 0, 0 };
                m_first8White[0b00010111] = { 1, 38, 8, 0, 0 };
                m_first8White[0b00101000] = { 1, 39, 8, 0, 0 };
                m_first8White[0b00101001] = { 1, 40, 8, 0, 0 };
                m_first8White[0b00101010] = { 1, 41, 8, 0, 0 };
                m_first8White[0b00101011] = { 1, 42, 8, 0, 0 };
                m_first8White[0b00101100] = { 1, 43, 8, 0, 0 };
                m_first8White[0b00101101] = { 1, 44, 8, 0, 0 };
                m_first8White[0b00000100] = { 1, 45, 8, 0, 0 };
                m_first8White[0b00000101] = { 1, 46, 8, 0, 0 };
                m_first8White[0b00001010] = { 1, 47, 8, 0, 0 };
                m_first8White[0b00001011] = { 1, 48, 8, 0, 0 };
                m_first8White[0b01010010] = { 1, 49, 8, 0, 0 };
                m_first8White[0b01010011] = { 1, 50, 8, 0, 0 };
                m_first8White[0b01010100] = { 1, 51, 8, 0, 0 };
                m_first8White[0b01010101] = { 1, 52, 8, 0, 0 };
                m_first8White[0b00100100] = { 1, 53, 8, 0, 0 };
                m_first8White[0b00100101] = { 1, 54, 8, 0, 0 };
                m_first8White[0b01011000] = { 1, 55, 8, 0, 0 };
                m_first8White[0b01011001] = { 1, 56, 8, 0, 0 };
                m_first8White[0b01011010] = { 1, 57, 8, 0, 0 };
                m_first8White[0b01011011] = { 1, 58, 8, 0, 0 };
                m_first8White[0b01001010] = { 1, 59, 8, 0, 0 };
                m_first8White[0b01001011] = { 1, 60, 8, 0, 0 };
                m_first8White[0b00110010] = { 1, 61, 8, 0, 0 };
                m_first8White[0b00110011] = { 1, 62, 8, 0, 0 };
                m_first8White[0b00110100] = { 1, 63, 8, 0, 0 };

                m_first8White[0b11011000] = { 1,  64, 5, 1, 0 };
                m_first8White[0b10010000] = { 1, 128, 5, 1, 0 };
                m_first8White[0b01011100] = { 1, 192, 6, 1, 0 };
                m_first8White[0b01101110] = { 1, 256, 7, 1, 0 };
                m_first8White[0b00110110] = { 1, 320, 8, 1, 0 };
                m_first8White[0b00110111] = { 1, 384, 8, 1, 0 };
                m_first8White[0b01100100] = { 1, 448, 8, 1, 0 };
                m_first8White[0b01100101] = { 1, 512, 8, 1, 0 };
                m_first8White[0b01101000] = { 1, 576, 8, 1, 0 };
                m_first8White[0b01100111] = { 1, 640, 8, 1, 0 };

                m_first8White[0b01100110] = { 1, 0, 0, 0, 0 }; m_final5White[0][0b00000] = { 1,  704, 9, 1, 0 };
                m_first8White[0b01100110] = { 1, 0, 0, 0, 0 }; m_final5White[0][0b10000] = { 1,  768, 9, 1, 0 };
                m_first8White[0b01101001] = { 1, 0, 0, 0, 1 }; m_final5White[1][0b00000] = { 1,  832, 9, 1, 0 };
                m_first8White[0b01101001] = { 1, 0, 0, 0, 1 }; m_final5White[1][0b10000] = { 1,  896, 9, 1, 0 };
                m_first8White[0b01101010] = { 1, 0, 0, 0, 2 }; m_final5White[2][0b00000] = { 1,  960, 9, 1, 0 };
                m_first8White[0b01101010] = { 1, 0, 0, 0, 2 }; m_final5White[2][0b10000] = { 1, 1024, 9, 1, 0 };
                m_first8White[0b01101011] = { 1, 0, 0, 0, 3 }; m_final5White[3][0b00000] = { 1, 1088, 9, 1, 0 };
                m_first8White[0b01101011] = { 1, 0, 0, 0, 3 }; m_final5White[3][0b10000] = { 1, 1152, 9, 1, 0 };
                m_first8White[0b01101100] = { 1, 0, 0, 0, 4 }; m_final5White[4][0b00000] = { 1, 1216, 9, 1, 0 };
                m_first8White[0b01101100] = { 1, 0, 0, 0, 4 }; m_final5White[4][0b10000] = { 1, 1280, 9, 1, 0 };
                m_first8White[0b01101101] = { 1, 0, 0, 0, 5 }; m_final5White[5][0b00000] = { 1, 1344, 9, 1, 0 };
                m_first8White[0b01101101] = { 1, 0, 0, 0, 5 }; m_final5White[5][0b10000] = { 1, 1408, 9, 1, 0 };
                m_first8White[0b01001100] = { 1, 0, 0, 0, 6 }; m_final5White[6][0b00000] = { 1, 1472, 9, 1, 0 };
                m_first8White[0b01001100] = { 1, 0, 0, 0, 6 }; m_final5White[6][0b10000] = { 1, 1536, 9, 1, 0 };
                m_first8White[0b01001101] = { 1, 0, 0, 0, 7 }; m_final5White[7][0b00000] = { 1, 1600, 9, 1, 0 };

                m_first8White[0b01100000] = { 1, 1664, 6, 1, 0 };

                m_first8White[0b01001101] = { 1, 0, 0, 0, 7 }; m_final5White[7][0b10000] = { 1, 1728,  9, 1, 0 };

                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b00000] = { 1, 1792, 11, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b10000] = { 1, 1856, 11, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b10100] = { 1, 1920, 11, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b00100] = { 1, 1984, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b00110] = { 1, 2048, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b01000] = { 1, 2112, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b01010] = { 1, 2176, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b01100] = { 1, 2240, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b01110] = { 1, 2304, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b11000] = { 1, 2368, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b11010] = { 1, 2432, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b11100] = { 1, 2496, 12, 1, 0 };
                m_first8White[0b00000001] = { 1, 0, 0, 0, 8 }; m_final5White[8][0b11110] = { 1, 2560, 12, 1, 0 };

                /* Fill 'em up! */
                MakeDense(m_first8Black);
                MakeDense(m_first8White);
                for (auto& arr : m_final5Black) MakeDense(arr, 8);
                for (auto& arr : m_final5White) MakeDense(arr, 8);
        }

    private:
        /*
                Example with N = 8 (3 total bits):

                IntegralPrefixes:

                        A: 100 (numBits = 1) // IntegralPrefix = "1"
                        B: 010 (numBits = 2) // IntegralPrefix = "01"

                Before MakeDense:

                        000 001 010 011 100 101 110 111
                         .   .   B   .   A   .   .   .

                After MakeDense:

                        000 001 010 011 100 101 110 111
                         .   .   B   B   A   A   A   A

                Now we can index for A using just the lower 1 bit, or for
                B using just the lower 2 bits, without the "extra" highest
                order bits interfering.
        */
        template<size_t N>
        static void MakeDense(std::array<IntegralPrefix, N>& arr, size_t bitOffset = 0)
        {
                static_assert(
                        (N > 1) && !(N & (N - 1)),
                        "Array length N must be a power of two.");

                static constexpr size_t TotalBits{ std::bit_width(N) - 1 };

                for (size_t i{}, trail{}, spanEnd{}; i < N; ++i)
                {
                        if (arr[i].live && arr[i].numBits)
                        {
                                assert(arr[i].numBits >= bitOffset);

                                const size_t effBits = size_t(arr[i].numBits) - bitOffset;
                                assert(effBits <= TotalBits);

                                spanEnd = i + (size_t{1} << (TotalBits - effBits));
                                trail = i;
                        }
                        else if (i < spanEnd)
                                arr[i] = arr[trail];
                }
        }

        std::array<IntegralPrefix, 256> m_first8Black{};
        std::array<IntegralPrefix, 256> m_first8White{};

        std::array<std::array<IntegralPrefix, 32>, 7> m_final5Black{};
        std::array<std::array<IntegralPrefix, 32>, 9> m_final5White{};
}; /* class IntegralPrefixTable */

class CodeWords
{
    public:
        using IntegralPrefix = IntegralPrefixTable::IntegralPrefix;
        using IntegralCode   = IntegralPrefixTable::IntegralCode;

        enum class Mode : int8_t
        {
                VerticalL3 = -3,
                VerticalL2 = -2,
                VerticalL1 = -1,
                Vertical0  =  0,
                VerticalR1 =  1,
                VerticalR2 =  2,
                VerticalR3 =  3,
                Horizontal =  4,
                Pass       =  5,
                Invalid    =  6
        }; /* enum class Mode */

        struct ModePrefix
        {
                Mode mode;
                uint16_t numBits;
        };

        struct ModeCode
        {
                uint16_t bits;
                uint16_t numBits;
        };

        static const IntegralPrefix& DecodeBlack13(size_t bits13)
        {
                return PrefixTable().DecodeBlack13(bits13);
        }

        static const IntegralPrefix& DecodeWhite13(size_t bits13)
        {
                return PrefixTable().DecodeWhite13(bits13);
        }

        static const IntegralCode& EncodeBlack(size_t value)
        {
                return IntegralPrefixTable::EncodeBlack(value);
        }

        static const IntegralCode& EncodeWhite(size_t value)
        {
                return IntegralPrefixTable::EncodeWhite(value);
        }

        static const ModePrefix& DecodeMode7(size_t bits7)
        {
                static constexpr ModePrefix verticalL3{ Mode::VerticalL3, 7 };
                static constexpr ModePrefix verticalL2{ Mode::VerticalL2, 6 };
                static constexpr ModePrefix verticalL1{ Mode::VerticalL1, 3 };
                static constexpr ModePrefix vertical0 { Mode::Vertical0,  1 };
                static constexpr ModePrefix verticalR1{ Mode::VerticalR1, 3 };
                static constexpr ModePrefix verticalR2{ Mode::VerticalR2, 6 };
                static constexpr ModePrefix verticalR3{ Mode::VerticalR3, 7 };
                static constexpr ModePrefix horizontal{ Mode::Horizontal, 3 };
                static constexpr ModePrefix pass      { Mode::Pass,       4 };
                static constexpr ModePrefix invalid   { Mode::Invalid,    0 };

                bits7 &= (1u << 7) - 1;

                /* lol */

                switch(bits7)
                {
                        case 0b0000000:
                        case 0b0000001:
                                return invalid;
                        case 0b0000010:
                                return verticalL3;
                        case 0b0000011:
                                return verticalR3;
                        case 0b0000100:
                        case 0b0000101:
                                return verticalL2;
                        case 0b0000110:
                        case 0b0000111:
                                return verticalR2;
                        case 0b0001000:
                        case 0b0001001:
                        case 0b0001010:
                        case 0b0001011:
                        case 0b0001100:
                        case 0b0001101:
                        case 0b0001110:
                        case 0b0001111:
                                return pass;
                        case 0b0010000:
                        case 0b0010001:
                        case 0b0010010:
                        case 0b0010011:
                        case 0b0010100:
                        case 0b0010101:
                        case 0b0010110:
                        case 0b0010111:
                        case 0b0011000:
                        case 0b0011001:
                        case 0b0011010:
                        case 0b0011011:
                        case 0b0011100:
                        case 0b0011101:
                        case 0b0011110:
                        case 0b0011111:
                                return horizontal;
                        case 0b0100000:
                        case 0b0100001:
                        case 0b0100010:
                        case 0b0100011:
                        case 0b0100100:
                        case 0b0100101:
                        case 0b0100110:
                        case 0b0100111:
                        case 0b0101000:
                        case 0b0101001:
                        case 0b0101010:
                        case 0b0101011:
                        case 0b0101100:
                        case 0b0101101:
                        case 0b0101110:
                        case 0b0101111:
                                return verticalL1;
                        case 0b0110000:
                        case 0b0110001:
                        case 0b0110010:
                        case 0b0110011:
                        case 0b0110100:
                        case 0b0110101:
                        case 0b0110110:
                        case 0b0110111:
                        case 0b0111000:
                        case 0b0111001:
                        case 0b0111010:
                        case 0b0111011:
                        case 0b0111100:
                        case 0b0111101:
                        case 0b0111110:
                        case 0b0111111:
                                return verticalR1;
                }

                return vertical0;
        }

        static const ModeCode& EncodeMode(Mode mode)
        {
                static constexpr std::array<ModeCode, 10> codes
                {
                        ModeCode{ 0b0000010, 7 }, // Vertical L3
                        ModeCode{ 0b000010,  6 }, // Vertical L2
                        ModeCode{ 0b010,     3 }, // Vertical L1
                        ModeCode{ 0b1,       1 }, // Vertical  0
                        ModeCode{ 0b011,     3 }, // Vertical R1
                        ModeCode{ 0b000011,  6 }, // Vertical R2
                        ModeCode{ 0b0000011, 7 }, // Vertical R3
                        ModeCode{ 0b001,     3 }, // Horizontal
                        ModeCode{ 0b0001,    4 }, // Pass
                        ModeCode{ 0b0,       0 }  // Invalid
                };

                /*
                        Enum starts at -3, but array starts at 0
                */
                uint8_t idx{ static_cast<uint8_t>(static_cast<int8_t>(mode) + 3) };

                assert(idx < codes.size());
                return codes[idx];
        }

    private:
        static const IntegralPrefixTable& PrefixTable()
        {
                static IntegralPrefixTable p{};
                return p;
        }
}; /* class CodeWords */

} /* namespace Gecko::Compression */