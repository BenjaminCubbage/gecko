#pragma once
#include <string>
#include <bitset>

namespace Gecko::API::Rand
{
    /*
        intent(ben): To generate RFC9562 V4 UUIDs.
    */
    class UUID
    {
      public:
        /*
            Generate a new V4 UUID.
        */
        static std::string GenerateUUID();

        /*
            Returns true if the string is a valid V4 UUID.
            Otherwise, false.
        */
        static bool IsValidV4UUID(const std::string& str);

        static constexpr size_t UUIDLength = 36;
    };
}