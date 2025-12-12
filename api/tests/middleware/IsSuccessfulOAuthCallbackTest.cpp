#include "gtest/gtest.h"
#include "gecko/middleware/IsSuccessfulOAuthCallback.h"
#include "httplib.h"
#include "json/json.h"
#include <string>

using namespace Gecko::API::Middleware;

namespace Gecko::API::Test
{
    TEST(IsSuccessfulOAuthCallback, ErrorParamReturnsFalseAndSetsOAuthCancelled)
    {
        httplib::Request  req;
        httplib::Response res;
        std::string       code{ "unchanged_code" };
        std::string       state{ "unchanged_state" };

        res.status = httplib::StatusCode::OK_200;

        req.params.emplace("error", "access_denied");
        req.params.emplace("code", "some_code");
        req.params.emplace("state", "some_state");

        IsSuccessfulOAuthCallback middleware;
        const bool ok = middleware(req, res, &code, &state);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        EXPECT_EQ(code, "unchanged_code");
        EXPECT_EQ(state, "unchanged_state");
    }

    TEST(IsSuccessfulOAuthCallback, MissingCodeReturnsFalseAndSetsOAuthCancelled)
    {
        httplib::Request  req;
        httplib::Response res;
        std::string       code{ "unchanged_code" };
        std::string       state{ "unchanged_state" };

        res.status = httplib::StatusCode::OK_200;

        req.params.emplace("state", "some_state");

        IsSuccessfulOAuthCallback middleware;
        const bool ok = middleware(req, res, &code, &state);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        EXPECT_EQ(code, "unchanged_code");
        EXPECT_EQ(state, "unchanged_state");
    }

    TEST(IsSuccessfulOAuthCallback, MissingStateReturnsFalseAndSetsOAuthCancelled)
    {
        httplib::Request  req;
        httplib::Response res;
        std::string       code{ "unchanged_code" };
        std::string       state{ "unchanged_state" };

        res.status = httplib::StatusCode::OK_200;

        req.params.emplace("code", "some_code");

        IsSuccessfulOAuthCallback middleware;
        const bool ok = middleware(req, res, &code, &state);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));

        EXPECT_EQ(code, "unchanged_code");
        EXPECT_EQ(state, "unchanged_state");
    }

    TEST(IsSuccessfulOAuthCallback, ValidCallbackReturnsTrueAndSetsOutputs)
    {
        httplib::Request  req;
        httplib::Response res;
        std::string       code;
        std::string       state;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string expectedCode{ "auth_code_123" };
        const std::string expectedState{ "xyz_state_456" };

        req.params.emplace("code", expectedCode);
        req.params.emplace("state", expectedState);

        IsSuccessfulOAuthCallback middleware;
        const bool ok = middleware(req, res, &code, &state);

        ASSERT_TRUE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());

        EXPECT_EQ(code, expectedCode);
        EXPECT_EQ(state, expectedState);
    }

    TEST(IsSuccessfulOAuthCallback, CanBeUsedMultipleTimes)
    {
        IsSuccessfulOAuthCallback middleware;

        // 1: valid
        {
            httplib::Request  req;
            httplib::Response res;
            std::string       code;
            std::string       state;

            res.status = httplib::StatusCode::OK_200;

            const std::string expectedCode{ "code1" };
            const std::string expectedState{ "state1" };

            req.params.emplace("code", expectedCode);
            req.params.emplace("state", expectedState);

            ASSERT_TRUE(middleware(req, res, &code, &state));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(code, expectedCode);
            EXPECT_EQ(state, expectedState);
        }

        // 2: error present
        {
            httplib::Request  req;
            httplib::Response res;
            std::string       code{ "unchanged_code" };
            std::string       state{ "unchanged_state" };

            res.status = httplib::StatusCode::OK_200;

            req.params.emplace("error", "access_denied");

            ASSERT_FALSE(middleware(req, res, &code, &state));
            EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400);
            EXPECT_FALSE(res.body.empty());
            EXPECT_EQ(code, "unchanged_code");
            EXPECT_EQ(state, "unchanged_state");
        }

        // 3: valid
        {
            httplib::Request  req;
            httplib::Response res;
            std::string       code;
            std::string       state;

            res.status = httplib::StatusCode::OK_200;

            const std::string expectedCode{ "code2" };
            const std::string expectedState{ "state2" };

            req.params.emplace("code", expectedCode);
            req.params.emplace("state", expectedState);

            ASSERT_TRUE(middleware(req, res, &code, &state));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(code, expectedCode);
            EXPECT_EQ(state, expectedState);
        }
    }
}