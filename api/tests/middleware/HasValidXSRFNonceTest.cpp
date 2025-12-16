#include "gtest/gtest.h"
#include "gecko/middleware/HasValidOAuthXSRFNonce.h"
#include "gecko/http/Constants.h"
#include "gecko/rand/UUID.h"
#include "httplib.h"
#include "json/json.h"
#include <string>

using namespace Gecko::API::Middleware;
using ::Gecko::API::Http::Constants::Cookies;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Test
{
    TEST(HasValidOAuthXSRFNonce, NoCookieHeaderReturnsFalseAndSetsXSRFMissing)
    {
        httplib::Request  req{};
        httplib::Response res{};

        res.status = httplib::StatusCode::OK_200;
        const std::string expected(Rand::UUID::UUIDLength, 'a');

        HasValidOAuthXSRFNonce middleware{};
        const bool ok = middleware(req, res, expected);

        ASSERT_FALSE(ok);

        // Missing cookie => XSRFMissing => 400 + non-empty body
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson{};
        Json::Reader reader{};
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidOAuthXSRFNonce, MissingNonceCookieReturnsFalseAndSetsXSRFMissing)
    {
        httplib::Request  req{};
        httplib::Response res{};

        res.status = httplib::StatusCode::OK_200;
        const std::string expected(Rand::UUID::UUIDLength, 'a');

        // Cookie header present but does not contain the OAuth XSRF nonce cookie
        const std::string cookieHeader = "other_cookie=abc123";
        req.set_header("Cookie", cookieHeader);

        HasValidOAuthXSRFNonce middleware{};
        const bool ok = middleware(req, res, expected);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson{};
        Json::Reader reader{};
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidOAuthXSRFNonce, NonceMismatchReturnsFalseAndSetsXSRFInvalid)
    {
        httplib::Request  req{};
        httplib::Response res{};

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{Cookies::HostHttpOAuthXSRFNonce};
        const std::string expected(Rand::UUID::UUIDLength, 'a'); // what we expect
        const std::string actual  (Rand::UUID::UUIDLength, 'b'); // what is in the cookie

        const std::string cookieHeader = cookieName + '=' + actual;
        req.set_header("Cookie", cookieHeader);

        HasValidOAuthXSRFNonce middleware{};
        const bool ok = middleware(req, res, expected);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson{};
        Json::Reader reader{};
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidOAuthXSRFNonce, NonceWrongLengthReturnsFalseAndSetsXSRFInvalid)
    {
        httplib::Request  req{};
        httplib::Response res{};

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{Cookies::HostHttpOAuthXSRFNonce};

        // Value matches expected string but has wrong length vs UUIDLength
        const std::string invalidLengthValue(Rand::UUID::UUIDLength + 1, 'a');
        const std::string expected = invalidLengthValue;

        const std::string cookieHeader = cookieName + '=' + invalidLengthValue;
        req.set_header("Cookie", cookieHeader);

        HasValidOAuthXSRFNonce middleware{};
        const bool ok = middleware(req, res, expected);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson{};
        Json::Reader reader{};
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidOAuthXSRFNonce, ValidNonceReturnsTrueAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string cookieName{Cookies::HostHttpOAuthXSRFNonce};
        const std::string expected(Rand::UUID::UUIDLength, 'a');

        const std::string cookieHeader = cookieName + '=' + expected;
        req.set_header("Cookie", cookieHeader);

        HasValidOAuthXSRFNonce middleware{};
        const bool ok = middleware(req, res, expected);

        ASSERT_TRUE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }
}