#include "gtest/gtest.h"
#include "gecko/http/UUID.h"
#include <cstddef>
#include <string>

using namespace Gecko::API::Http;

namespace Gecko::API::Test
{
    // Basic shape: 8-4-4-4-12, lowercase hex, dashes in the right spots.
    TEST(UUID, FormattedCorrectly)
    {
        const std::string uuid = UUID::GenerateUUID();

        ASSERT_EQ(uuid.size(), UUID::UUIDLength);

        // Dashes
        ASSERT_EQ(uuid[8],  '-');
        ASSERT_EQ(uuid[13], '-');
        ASSERT_EQ(uuid[18], '-');
        ASSERT_EQ(uuid[23], '-');

        // All other characters are lowercase hex digits
        for (std::size_t i = 0; i < uuid.size(); ++i)
        {
            if (i == 8 || i == 13 || i == 18 || i == 23) continue;

            const char c = uuid[i];
            const bool isDigit    = (c >= '0' && c <= '9');
            const bool isLowerHex = (c >= 'a' && c <= 'f');

            ASSERT_TRUE(isDigit || isLowerHex)
                << "Non-hex character '" << c << "' at index " << i;
        }
    }

    // RFC9562 / RFC4122 v4: version nibble = 4 (third group, first char).
    TEST(UUID, VersionIs4)
    {
        const std::string uuid = UUID::GenerateUUID();
        ASSERT_EQ(uuid.size(), UUID::UUIDLength);

        // Groups: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        // Index 14 = first hex digit of the 3rd group.
        ASSERT_EQ(uuid[14], '4');
    }

    // RFC9562 / RFC4122 variant 1: 10xx in the variant bits => 8,9,a,b.
    TEST(UUID, VariantIsRFC4122)
    {
        const std::string uuid = UUID::GenerateUUID();
        ASSERT_EQ(uuid.size(), UUID::UUIDLength);

        // Index 19 = first hex digit of the 4th group.
        const char variant = uuid[19];
        ASSERT_TRUE(variant == '8' ||
                    variant == '9' ||
                    variant == 'a' ||
                    variant == 'b');
    }
}