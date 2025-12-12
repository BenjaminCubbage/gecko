#include "gtest/gtest.h"
#include "gecko/middleware/HasJSONValueMember.h"
#include "httplib.h"
#include "json/json.h"

using namespace Gecko::API::Middleware;

namespace Gecko::API::Test
{
    TEST(HasJSONValueMember_String, ValidFieldReturnsTrueAndLeavesResponseEmpty)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        std::string       value;

        res.status = httplib::StatusCode::OK_200;
        body["username"] = "gecko";

        HasJSONValueMember<std::string> hasUsername{ "username" };
        const bool ok = hasUsername(req, res, body, &value);

        ASSERT_TRUE(ok);

        // Middleware success: response should still be "default"
        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());

        // Out value should be set correctly
        EXPECT_EQ(value, "gecko");
    }

    TEST(HasJSONValueMember_String, MissingFieldReturnsFalseAndSetsMissingFieldError)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        std::string       value = "unchanged";

        res.status = httplib::StatusCode::OK_200;
        // Note: body has no "username" field

        HasJSONValueMember<std::string> hasUsername{ "username" };
        const bool ok = hasUsername(req, res, body, &value);

        ASSERT_FALSE(ok);

        // Missing field => UnprocessableContent_422
        EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
        EXPECT_FALSE(res.body.empty());

        // Optionally verify top-level "error" exists
        Json::Value errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        // Out value should not have been modified
        EXPECT_EQ(value, "unchanged");
    }

    TEST(HasJSONValueMember_String, WrongTypeReturnsFalseAndSetsError)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        std::string       value = "unchanged";

        res.status = httplib::StatusCode::OK_200;
        body["username"] = 123; // int, but we expect string

        HasJSONValueMember<std::string> hasUsername{ "username"};
        const bool ok = hasUsername(req, res, body, &value);

        ASSERT_FALSE(ok);

        // On failure, middleware should set an error response
        EXPECT_NE(res.status, httplib::StatusCode::OK_200);
        EXPECT_FALSE(res.body.empty());

        Json::Value errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        // Out value should not have been modified
        EXPECT_EQ(value, "unchanged");
    }

    TEST(HasJSONValueMember_String, CanBeUsedMultipleTimes)
    {
        HasJSONValueMember<std::string> hasUsername{ "username" };

        // 1: valid
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       body;
            std::string       value;

            res.status = httplib::StatusCode::OK_200;
            body["username"] = "first";

            ASSERT_TRUE(hasUsername(req, res, body, &value));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(value, "first");
        }

        // 2: missing field
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       body;
            std::string       value = "unchanged";

            res.status = httplib::StatusCode::OK_200;
            // no "username"

            ASSERT_FALSE(hasUsername(req, res, body, &value));
            EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
            EXPECT_FALSE(res.body.empty());
            EXPECT_EQ(value, "unchanged");
        }

        // 3: valid again
        {
            httplib::Request  req;
            httplib::Response res;
            Json::Value       body;
            std::string       value;

            res.status = httplib::StatusCode::OK_200;
            body["username"] = "third";

            ASSERT_TRUE(hasUsername(req, res, body, &value));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(value, "third");
        }
    }

    TEST(HasJSONValueMember_Int, ValidFieldReturnsTrueAndLeavesResponseEmpty)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        int               value = 0;

        res.status = httplib::StatusCode::OK_200;
        body["id"] = 42;

        HasJSONValueMember<int> hasID{ "id" };
        const bool ok = hasID(req, res, body, &value);

        ASSERT_TRUE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
        EXPECT_EQ(value, 42);
    }

    TEST(HasJSONValueMember_Int, MissingFieldReturnsFalseAndSetsMissingFieldError)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        int               value = -1;

        res.status = httplib::StatusCode::OK_200;
        // Note: body has no "id" field

        HasJSONValueMember<int> hasID{ "id" };
        const bool ok = hasID(req, res, body, &value);

        ASSERT_FALSE(ok);

        // Missing field => UnprocessableContent_422
        EXPECT_EQ(res.status, httplib::StatusCode::UnprocessableContent_422);
        EXPECT_FALSE(res.body.empty());

        Json::Value errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        // Out value should not have been modified
        EXPECT_EQ(value, -1);
    }

    TEST(HasJSONValueMember_Int, WrongTypeReturnsFalseAndSetsError)
    {
        httplib::Request  req;
        httplib::Response res;
        Json::Value       body;
        int               value = -1;

        res.status = httplib::StatusCode::OK_200;
        body["id"] = "not-an-int"; // string, but we expect int

        HasJSONValueMember<int> hasID("id");
        const bool ok = hasID(req, res, body, &value);

        ASSERT_FALSE(ok);

        // On failure, middleware should set an error response
        EXPECT_NE(res.status, httplib::StatusCode::OK_200);
        EXPECT_FALSE(res.body.empty());

        Json::Value errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        // Out value should not have been modified
        EXPECT_EQ(value, -1);
    }
}