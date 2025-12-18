#include "gtest/gtest.h"
#include "gecko/middleware/HasPathParam.h"
#include "httplib.h"
#include "json/json.h"
#include <cstdint>
#include <string>

namespace Http = ::Gecko::API::Http;

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

    // -------------------------
    // String / string_view path params
    // -------------------------

    TEST(HasPathParam_String, MissingParamReturnsFalseAndSetsMalformedPath)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        Gecko::API::Middleware::HasPathParam<std::string> mw{ "userId", 16 };

        std::string out = "sentinel";
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400); // adjust if MalformedPath differs
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_String, EmptyParamReturnsFalseAndSetsMalformedPath)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.path_params.emplace("userId", "");

        Gecko::API::Middleware::HasPathParam<std::string> mw{ "userId", 16 };

        std::string out = "sentinel";
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_String, TooLongReturnsFalseAndSetsPayloadTooLarge)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.path_params.emplace("userId", "123456789"); // 9 chars

        Gecko::API::Middleware::HasPathParam<std::string> mw{ "userId", 8 };

        std::string out = "sentinel";
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::PayloadTooLarge_413); // adjust if PayloadTooLarge differs
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_String, ValidReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.path_params.emplace("userId", "abc123");

        Gecko::API::Middleware::HasPathParam<std::string> mw{ "userId", 16 };

        std::string out;
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, "abc123");
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    // -------------------------
    // Unsigned integral path params
    // -------------------------

    TEST(HasPathParam_U32, MissingParamReturnsFalseAndSetsMalformedPath)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        Gecko::API::Middleware::HasPathParam<uint32_t> mw{ "id" };

        uint32_t out = 123;
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_U32, NonNumericReturnsFalseAndSetsUnprocessable)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.path_params.emplace("id", "12x3");

        Gecko::API::Middleware::HasPathParam<uint32_t> mw{ "id" };

        uint32_t out = 999;
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422); // adjust if Unprocessable differs
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_U32, TooManyDigitsReturnsFalseAndSetsUnprocessable)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        // For uint32_t, digits10 = 9, so max allowed size is 10. Use 11 to force fail.
        req.path_params.emplace("id", "12345678901");

        Gecko::API::Middleware::HasPathParam<uint32_t> mw{ "id" };

        uint32_t out = 999;
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
        ExpectErrorJson(res);
    }

    TEST(HasPathParam_U32, ValidReturnsTrueParsesAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.path_params.emplace("id", "42");

        Gecko::API::Middleware::HasPathParam<uint32_t> mw{ "id" };

        uint32_t out = 0;
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 42u);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasPathParam_U32, ZeroIsAllowed)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.path_params.emplace("id", "0");

        Gecko::API::Middleware::HasPathParam<uint32_t> mw{ "id" };

        uint32_t out = 123;
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, 0u);
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }
}