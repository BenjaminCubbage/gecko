#include "gtest/gtest.h"
#include "gecko/middleware/HasHeader.h"
#include "httplib.h"
#include "json/json.h"
#include <string>

using ::Gecko::API::Middleware::HasHeader;

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

    TEST(HasHeader, MissingHeaderReturnsFalseAndSetsMissingIdempotencyKey)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        HasHeader mw{ "Idempotency-Key" };

        std::string out = "sentinel";
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_TRUE(out.empty());

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasHeader, EmptyHeaderReturnsFalseAndSetsMissingIdempotencyKey)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        req.set_header("Idempotency-Key", "");

        HasHeader mw{ "Idempotency-Key" };

        std::string out = "sentinel";
        const bool ok = mw(req, res, &out);

        ASSERT_FALSE(ok);
        EXPECT_TRUE(out.empty());

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        ExpectErrorJson(res);
    }

    TEST(HasHeader, PresentHeaderReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.set_header("Idempotency-Key", "abc123");

        HasHeader mw{ "Idempotency-Key" };

        std::string out;
        const bool ok = mw(req, res, &out);

        ASSERT_TRUE(ok);
        EXPECT_EQ(out, "abc123");
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }
}
