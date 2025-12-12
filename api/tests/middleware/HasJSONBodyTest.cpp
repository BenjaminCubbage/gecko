#include "gtest/gtest.h"
#include "gecko/middleware/HasJSONBody.h"
#include "json/json.h"
#include "httplib.h"

using namespace Gecko::API::Middleware;

namespace Gecko::API::Test
{
    TEST(HasJSONBody, ValidJSONReturnsTrueAndLeavesResponseEmpty)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       parsed;

        res.status = httplib::StatusCode::OK_200;
        req.body = R"({"username":"gecko","id":123})";

        HasJSONBody hasJSONBody;
        const bool ok = hasJSONBody(req, res, &parsed);

        ASSERT_TRUE(ok);

        // Middleware success: response should still be "default"
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());

        // Parsed JSON should be an object with the expected fields
        ASSERT_TRUE(parsed.isObject());
        ASSERT_TRUE(parsed.isMember("username"));
        EXPECT_EQ(parsed["username"].asString(), "gecko");
        ASSERT_TRUE(parsed.isMember("id"));
        EXPECT_EQ(parsed["id"].asInt(), 123);
    }

    TEST(HasJSONBody, InvalidJSONReturnsFalseAndSetsUnprocessableError)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       parsed;

        req.body = "{\"invalid JSON\":}";

        HasJSONBody hasJSONBody;
        const bool ok = hasJSONBody(req, res, &parsed);

        ASSERT_FALSE(ok);

        // On failure, middleware should set an Unprocessable error response
        EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
        EXPECT_FALSE(res.body.empty());

        // Optionally verify top-level "error" exists in the returned JSON body
        Json::Value errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(HasJSONBody, CanBeUsedMultipleTimes)
    {
        HasJSONBody hasJSONBody;

        // 1
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       parsed;

            res.status = httplib::StatusCode::OK_200;
            req.body = R"({"first":true})";

            ASSERT_TRUE(hasJSONBody(req, res, &parsed));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            ASSERT_TRUE(parsed["first"].asBool());
        }

        // 2
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       parsed;

            res.status = httplib::StatusCode::OK_200;
            req.body = "{ invalid";

            ASSERT_FALSE(hasJSONBody(req, res, &parsed));
            EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
            EXPECT_FALSE(res.body.empty());
        }

        // 3
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       parsed;

            res.status = httplib::StatusCode::OK_200;
            req.body = R"({"third":3})";
            
            ASSERT_TRUE(hasJSONBody(req, res, &parsed));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            ASSERT_EQ(parsed["third"].asInt(), 3);
        }
    }
}