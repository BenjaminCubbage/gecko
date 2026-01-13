#include "gtest/gtest.h"
#include <limits>
#include "gecko/middleware/HasContentLengthLTE.h"
#include "httplib.h"
#include "json/json.h"

using ::Gecko::API::Middleware::HasContentLengthLTE;


namespace Gecko::API::Test
{
    namespace 
    {
        void ExpectErrorJson(const httplib::Response& res)
        {
            ASSERT_FALSE(res.body.empty());
            Json::Value  j{};
            Json::Reader r{};
            ASSERT_TRUE(r.parse(res.body, j));
            ASSERT_TRUE(j.isMember("error"));
        }
    }

    TEST(HasContentLengthLTE, MissingHeaderReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        size_t out = 123;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::LengthRequired_411);
        ExpectErrorJson(res);
    }

    TEST(HasContentLengthLTE, TooManyDigitsReturnsFalseAndSetsBadRequest)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "00000000000000000000000000000000001");

        size_t out = 123;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLengthLTE, NonDigitReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "12a3");

        size_t out = 123;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLengthLTE, ZeroReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "0");

        size_t out = 123;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLengthLTE, DoubleZeroReturnsFalseAndSetsBadContentLength)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Content-Length", "00");

        size_t out = 123;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasContentLengthLTE, ValidLengthReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.set_header("Content-Length", "42");

        size_t out = 0;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 10000, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 42);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasContentLengthLTE, LessThanMaxAcceptedAndParsedCorrectly)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();
        
        req.set_header("Content-Length", "9999998");

        size_t out = 0;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 9999999, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 9999998);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasContentLengthLTE, EqualToMaxAccepted)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();
        
        req.set_header("Content-Length", "9999999");

        size_t out = 0;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 9999999, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 9999999);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasContentLengthLTE, GreaterThanMaxRejectedAndSetsPayloadTooLarge)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();
        
        req.set_header("Content-Length", "9999999");

        size_t out = 0;

        HasContentLengthLTE mw{};
        const bool ok = mw(req, res, 9999998, &out);

        ASSERT_FALSE(ok);
        ASSERT_EQ(res.status, httplib::StatusCode::PayloadTooLarge_413);
    }
}