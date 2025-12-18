#include "gtest/gtest.h"
#include "gecko/middleware/HasContentLength.h"
#include "httplib.h"
#include "json/json.h"

using ::Gecko::API::Middleware::HasContentLength;

namespace Gecko::API::Test
{
    static void ExpectErrorJson(const httplib::Response& res)
    {
        ASSERT_FALSE(res.body.empty());
        Json::Value  j{};
        Json::Reader r{};
        ASSERT_TRUE(r.parse(res.body, j));
        ASSERT_TRUE(j.isMember("error"));
    }

    TEST(HasContentLength, MissingHeaderReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        size_t out = 123;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLength, TooManyDigitsReturnsFalseAndSetsPayloadTooLarge)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        // note(ben): 8 digits not allowed
        req.set_header("Content-Length", "10000000");

        size_t out = 123;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::PayloadTooLarge_413);
        ExpectErrorJson(res);
    }

    TEST(HasContentLength, NonDigitReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "12a3");

        size_t out = 123;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLength, ZeroReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "0");

        size_t out = 123;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLength, DoubleZeroReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "00");

        size_t out = 123;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLength, ValidLengthReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.set_header("Content-Length", "42");

        size_t out = 0;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 42u);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasContentLength, MaxSevenDigitsParsesCorrectly)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();
        
        // note(ben): 7 digits OK
        req.set_header("Content-Length", "9999999");

        size_t out = 0;

        HasContentLength mw{};
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 9999999u);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }
}