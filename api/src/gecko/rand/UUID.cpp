#include "gecko/rand/UUID.h"
#include <array>
#include <random>

namespace Gecko::API::Rand
{
    std::string UUID::GenerateUUID()
    {
        thread_local std::mt19937_64 rng(std::random_device{}());
        thread_local std::array<uint16_t, 8> blocks{};

        uint64_t r1 = rng();
        uint64_t r2 = rng();

        blocks[0] = static_cast<uint16_t>(r1);
        blocks[1] = static_cast<uint16_t>(r1 >> 16);
        blocks[2] = static_cast<uint16_t>(r1 >> 32);
        blocks[3] = static_cast<uint16_t>(r1 >> 48);
        blocks[4] = static_cast<uint16_t>(r2);
        blocks[5] = static_cast<uint16_t>(r2 >> 16);
        blocks[6] = static_cast<uint16_t>(r2 >> 32);
        blocks[7] = static_cast<uint16_t>(r2 >> 48);

        blocks[3] = (blocks[3] & 0x0FFF) | 0x4000; /* V4 */
        blocks[4] = (blocks[4] & 0x3FFF) | 0x8000; /* Variant */

        std::string result;
        result.reserve(UUIDLength);

        const auto appendHex = [&result](uint16_t block) {
            static constexpr const char* digits = "0123456789abcdef";
            result.push_back(digits[(block >> 12) & 0xF]);
            result.push_back(digits[(block >> 8 ) & 0xF]);
            result.push_back(digits[(block >> 4 ) & 0xF]);
            result.push_back(digits[(block >> 0 ) & 0xF]);
        };

        appendHex(blocks[0]);
        appendHex(blocks[1]);
        result.push_back('-');
        appendHex(blocks[2]);
        result.push_back('-');
        appendHex(blocks[3]);
        result.push_back('-');
        appendHex(blocks[4]);
        result.push_back('-');
        appendHex(blocks[5]);
        appendHex(blocks[6]);
        appendHex(blocks[7]);

        return result;
    }

    bool UUID::IsValidV4UUID(const std::string& str)
    {
        const auto isHexDigit = [] (char c) {
            return (c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'f') ||
                   (c >= 'A' && c <= 'F');
        };

        if (str.size() != UUIDLength)
            return false;

        for (size_t i = 0; i < str.size(); ++i)
        {
            bool hyphen = i == 8 ||
                          i == 13 ||
                          i == 18 ||
                          i == 23;

            if (hyphen)
            {
                if (str[i] != '-')
                    return false;
            }
            else
            {
                if (!isHexDigit(str[i]))
                    return false;
            }
        }

        const char variant = str[19];

        if (variant != '8' &&
            variant != '9' &&
            variant != 'a' &&
            variant != 'b') // Variant nibble
            return false;

        return str[14] == '4'; // V4
    }
}