#include "gtest/gtest.h"
#include "gecko/http/ParseHeader.h"
#include <optional>

using namespace Gecko::API::Http;

namespace Gecko::API::Test
{
    TEST(ParseHeader, ReturnsNullOptCookieValueForEmptyArgs)
    {
        EXPECT_FALSE(!!ParseHeader::GetCookieValue("H1=abcdefg; H2=abc", ""));
        EXPECT_FALSE(!!ParseHeader::GetCookieValue("", "H1"));
        EXPECT_FALSE(!!ParseHeader::GetCookieValue("", ""));
    }

    TEST(ParseHeader, ReturnsCorrectCookieValueFromMultiplePossibilities)
    {
        const auto value = ParseHeader::GetCookieValue("H1=abcdefg; H2=hijklmn; H3=opqrstu", "H2");
        EXPECT_TRUE(value);
        EXPECT_EQ(*value, "hijklmn");
    }

    TEST(ParseHeader, ReturnsCorrectCookieValueFromOnePossibility)
    {
        const auto value = ParseHeader::GetCookieValue("H1=abcdefg", "H1");
        EXPECT_TRUE(value);
        EXPECT_EQ(*value, "abcdefg");
    }

    TEST(ParseHeader, ReturnsNullOptForNoMatch)
    {
        EXPECT_FALSE(ParseHeader::GetCookieValue("H1=abcdefg", "H2"));
    }

    TEST(ParseHeader, HandlesWhitespaceGracefully)
    {
        const auto value = ParseHeader::GetCookieValue("           H1=abcdefg;           H2=hijklmn;        H3=opqrstu", "H2");

        EXPECT_TRUE(value);
        EXPECT_EQ(*value, "hijklmn");
    }

    TEST(ParseHeader, HandlesTrailingSemicolonGracefully)
    {
        const auto value = ParseHeader::GetCookieValue("H1=abcdefg; H2=hijklmn; H3=opqrstu;", "H3");

        EXPECT_TRUE(value);
        EXPECT_EQ(*value, "opqrstu");
    }
}