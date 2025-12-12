#include "gtest/gtest.h"
#include "gecko/middleware/PathParamEquals.h"
#include "httplib.h"
#include "json/json.h"

using namespace Gecko::API::Middleware;

namespace Gecko::API::Test
{
    TEST(PathParamEquals, MissingParamReturnsFalseAndSetsForbidden)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        PathParamEquals middleware{ "id" };
        const bool ok = middleware(req, res, "123");

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Forbidden_403);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(PathParamEquals, WrongParamValueReturnsFalseAndSetsForbidden)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        req.path_params.emplace("id", "999");

        PathParamEquals middleware{ "id" };
        const bool ok = middleware(req, res, "123");

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Forbidden_403);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(PathParamEquals, ParamWithDifferentNameIsIgnoredAndReturnsForbidden)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        req.path_params.emplace("user_id", "123");

        PathParamEquals middleware{ "id" };
        const bool ok = middleware(req, res, "123");

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Forbidden_403);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(PathParamEquals, MatchingParamReturnsTrueAndLeavesResponseUntouched)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        req.path_params.emplace("id", "123");

        PathParamEquals middleware{ "id" };
        const bool ok = middleware(req, res, "123");

        ASSERT_TRUE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(PathParamEquals, CanBeUsedMultipleTimes)
    {
        PathParamEquals middleware{ "id" };

        // 1: valid
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            req.path_params.emplace("id", "1");

            ASSERT_TRUE(middleware(req, res, "1"));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
        }

        // 2: missing
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            ASSERT_FALSE(middleware(req, res, "2"));
            EXPECT_EQ(res.status, httplib::StatusCode::Forbidden_403);
            EXPECT_FALSE(res.body.empty());
        }

        // 3: wrong value
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            req.path_params.emplace("id", "3");

            ASSERT_FALSE(middleware(req, res, "4"));
            EXPECT_EQ(res.status, httplib::StatusCode::Forbidden_403);
            EXPECT_FALSE(res.body.empty());
        }

        // 4: valid
        {
            httplib::Request  req;
            httplib::Response res;

            res.status = httplib::StatusCode::OK_200;

            req.path_params.emplace("id", "5");

            ASSERT_TRUE(middleware(req, res, "5"));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
        }
    }
}