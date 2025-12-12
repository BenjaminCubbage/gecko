#include "gtest/gtest.h"
#include "gecko/http/RespondWithError.h"
#include "json/json.h"
#include "httplib.h"

using namespace Gecko::API::Http;

namespace Gecko::API::Test
{
    TEST(RespondWithError, RespondsWithWellFormedErrorJson)
    {
        httplib::Response response;
        Json::Value json;
        Json::Reader reader;

        // Has error field, with code and reason fields nested.
        RespondWithError::CouldNotFulfill(response);
        ASSERT_TRUE(reader.parse(response.body, json));
        ASSERT_TRUE(json.isMember("error") && json["error"].isObject());
        ASSERT_TRUE(json["error"].isMember("code")   && json["error"]["code"].isString());
        ASSERT_TRUE(json["error"].isMember("reason") && json["error"]["reason"].isString());

        // This type of response should also have a "field" sub-member
        RespondWithError::MissingField(response, "field");
        ASSERT_TRUE(reader.parse(response.body, json));
        ASSERT_TRUE(json.isMember("error") && json["error"].isObject());
        ASSERT_TRUE(json["error"].isMember("code")   && json["error"]["code"].isString());
        ASSERT_TRUE(json["error"].isMember("reason") && json["error"]["reason"].isString());
        ASSERT_TRUE(json["error"].isMember("field")  && json["error"]["field"].isString());
    }
}