#include "gtest/gtest.h"
#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/http/Constants.h"
#include "gecko/rand/UUID.h"
#include "httplib.h"
#include "json/json.h"
#include <string>

using namespace Gecko::API::Middleware;
using ::Gecko::API::Http::Constants::Cookies;
using ::Gecko::API::Http::Constants::Headers;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Test
{
    TEST(HasValidXSRFToken, NoCookieHeaderReturnsFalseAndSetsXSRFMissing)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidXSRFToken, MissingXSRFCookieReturnsFalseAndSetsXSRFMissing)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieHeader = "other_cookie=abc123";
        req.set_header("Cookie", cookieHeader);

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidXSRFToken, MissingXSRFHeaderReturnsFalseAndSetsXSRFMissing)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{ Cookies::HostXSRFToken };
        const std::string token(Rand::UUID::UUIDLength, 'a');

        const std::string cookieHeader = cookieName + '=' + token;
        req.set_header("Cookie", cookieHeader);

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidXSRFToken, TokenMismatchReturnsFalseAndSetsXSRFInvalid)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{ Cookies::HostXSRFToken };
        const std::string cookieValue(Rand::UUID::UUIDLength, 'a');
        const std::string headerValue(Rand::UUID::UUIDLength, 'b');

        const std::string cookieHeader = cookieName + '=' + cookieValue;
        req.set_header("Cookie", cookieHeader);
        req.set_header(Headers::XXSRFToken, headerValue);

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidXSRFToken, HeaderWrongLengthReturnsFalseAndSetsXSRFInvalid)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{ Cookies::HostXSRFToken };

        // Cookie and header values match each other, but length != UUIDLength
        const std::string token(Rand::UUID::UUIDLength + 1, 'a');

        const std::string cookieHeader = cookieName + '=' + token;
        req.set_header("Cookie", cookieHeader);
        req.set_header(Headers::XXSRFToken, token);

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasValidXSRFToken, ValidTokensReturnTrueAndLeaveResponseUntouched)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string cookieName{ Cookies::HostXSRFToken };
        const std::string token(Rand::UUID::UUIDLength, 'a');

        const std::string cookieHeader = cookieName + '=' + token;
        req.set_header("Cookie", cookieHeader);
        req.set_header(Headers::XXSRFToken, token);

        HasValidXSRFToken middleware;
        const bool ok = middleware(req, res);

        ASSERT_TRUE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasValidXSRFToken, CanBeUsedMultipleTimes)
    {
        HasValidXSRFToken middleware;
        const std::string cookieName{ Cookies::HostXSRFToken };

        // 1: valid
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            const std::string token(Rand::UUID::UUIDLength, 'a');
            const std::string cookieHeader = cookieName + '=' + token;
            req.set_header("Cookie", cookieHeader);
            req.set_header(Headers::XXSRFToken, token);

            ASSERT_TRUE(middleware(req, res));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
        }

        // 2: missing header
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            const std::string token(Rand::UUID::UUIDLength, 'a');
            const std::string cookieHeader = cookieName + '=' + token;
            req.set_header("Cookie", cookieHeader);

            ASSERT_FALSE(middleware(req, res));
            EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
            EXPECT_FALSE(res.body.empty());
        }

        // 3: valid
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            const std::string token(Rand::UUID::UUIDLength, 'b');
            const std::string cookieHeader = cookieName + '=' + token;
            req.set_header("Cookie", cookieHeader);
            req.set_header(Headers::XXSRFToken, token);

            ASSERT_TRUE(middleware(req, res));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
        }
    }
}