#pragma once
#include <string>
#include <bitset>

namespace Gecko::API::Rand
{
    class UUID
    {
    public:
        // Generate a RFC9562 V4 UUID
        static std::string GenerateUUID();

        // As per spec, uppercase okay
        static bool IsValidV4UUID(const std::string& str);

        static constexpr size_t UUIDLength = 36;
    };
}