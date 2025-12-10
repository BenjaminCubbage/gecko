#pragma once
#include <string>

namespace Gecko::API::Http
{
    class UUID
    {
    public:
        // Generate a RFC9562 V4 UUID
        static std::string GenerateUUID();
        static constexpr size_t UUIDLength = 36;
    };
}