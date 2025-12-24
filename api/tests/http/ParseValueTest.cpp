#include "gtest/gtest.h"
#include "gecko/http/ParseValue.h"
#include <cstdint>
#include <string_view>

namespace Gecko::API::Test
{
    using Gecko::API::Http::ParseValue;

    TEST(ParseValue, ParsesBasicValues)
    {
        std::uint8_t out = 0;

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint8_t>("0", &out));
        EXPECT_EQ(out, 0);

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint8_t>("7", &out));
        EXPECT_EQ(out, 7);

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint8_t>("255", &out));
        EXPECT_EQ(out, 255);
    }

    TEST(ParseValue, RejectsEmptyAndNonDigits)
    {
        std::uint8_t out = 123;

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>(" ", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("1 ", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>(" 1", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("1a", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("-1", &out));
        EXPECT_EQ(out, 123);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("+1", &out));
        EXPECT_EQ(out, 123);
    }

    TEST(ParseValue, AllowsLeadingZeros)
    {
        std::uint8_t out = 99;

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint8_t>("000", &out));
        EXPECT_EQ(out, 0);

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint8_t>("007", &out));
        EXPECT_EQ(out, 7);
    }

    TEST(ParseValue, RejectsOverflow)
    {
        std::uint8_t out = 200;

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("256", &out));
        EXPECT_EQ(out, 200);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("999", &out));
        EXPECT_EQ(out, 200);
    }

    TEST(ParseValue, RejectsTooManyDigits)
    {
        std::uint8_t out = 42;

        // uint8_t maxDigits = digits10 + 1 = 2 + 1 = 3
        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("0000", &out));
        EXPECT_EQ(out, 42);

        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint8_t>("1234", &out));
        EXPECT_EQ(out, 42);
    }

    TEST(ParseValue, ParsesAndRejectsKnownBoundaries)
    {
        std::uint16_t out = 0;

        ASSERT_TRUE(ParseValue::TryParseIntegral<std::uint16_t>("65535", &out));
        EXPECT_EQ(out, 65535);

        out = 1234;
        EXPECT_FALSE(ParseValue::TryParseIntegral<std::uint16_t>("65536", &out));
        EXPECT_EQ(out, 1234);
    }
}