#include "gtest/gtest.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/http/Constants.h"
#include "httplib.h"
#include "json/json.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include <string>

using namespace Gecko::API::Middleware;
using ::Gecko::API::Http::Constants::Cookies;
using ::Gecko::API::Http::Constants::Issuer;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Test
{
    namespace
    {
        // ES256 test keypair (public for middleware, private for signing)
        const std::string kEcPrivateKey{ R"(-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgPGJGAm4X1fvBuC1z
SpO/4Izx6PXfNMaiKaS5RUkFqEGhRANCAARCBvmeksd3QGTrVs2eMrrfa7CYF+sX
sjyGg+Bo5mPKGH4Gs8M7oIvoP9pb/I85tdebtKlmiCZHAZE5w4DfJSV6
-----END PRIVATE KEY-----)" };

        const std::string kEcPublicKey{ R"(-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEQgb5npLHd0Bk61bNnjK632uwmBfr
F7I8hoPgaOZjyhh+BrPDO6CL6D/aW/yPObXXm7SpZogmRwGROcOA3yUleg==
-----END PUBLIC KEY-----)" };

        using JsonTraits = jwt::traits::open_source_parsers_jsoncpp;
    }

    //
    // Common error-path tests (string overload)
    //

    TEST(UserIsLoggedIn, NoCookieHeaderReturnsFalseAndSetsAuthMissing)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID;

        const bool ok = middleware(req, res, &userID);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(UserIsLoggedIn, MissingAuthCookieReturnsFalseAndSetsAuthMissing)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        // Some other cookie, but not the Gecko auth cookie
        req.set_header("Cookie", "other=abc123");

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID;

        const bool ok = middleware(req, res, &userID);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    TEST(UserIsLoggedIn, InvalidTokenReturnsFalseAndSetsAuthInvalid)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        const std::string cookieName{ Cookies::HostHttpGeckoAuth };
        const std::string badToken{ "this-is-not-a-jwt" };

        req.set_header("Cookie", cookieName + '=' + badToken);

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID;

        const bool ok = middleware(req, res, &userID);

        ASSERT_FALSE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
    }

    //
    // Success path (string overload)
    //

    TEST(UserIsLoggedIn, ValidTokenReturnsTrueAndSetsStringUserID)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string cookieName{ Cookies::HostHttpGeckoAuth };
        const int expectedUserID{ 123 };

        const auto token =
            jwt::create<JsonTraits>()
                .set_issuer(Issuer::GeckoIssuerName)
                .set_subject(std::to_string(expectedUserID))
                .sign(jwt::algorithm::es256{ "", kEcPrivateKey });

        req.set_header("Cookie", cookieName + '=' + token);

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID;

        const bool ok = middleware(req, res, &userID);

        ASSERT_TRUE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
        EXPECT_EQ(userID, expectedUserID);
    }

    //
    // Int overload: basic success and passthrough
    //

    TEST(UserIsLoggedIn, IntOverloadValidNumericSubjectSetsIntUserID)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string cookieName{ Cookies::HostHttpGeckoAuth };
        const std::string subjectStr{ "42" };
        const int expectedUserID = 42;

        const auto token =
            jwt::create<JsonTraits>()
                .set_issuer(Issuer::GeckoIssuerName)
                .set_subject(subjectStr)
                .sign(jwt::algorithm::es256{ "", kEcPrivateKey });

        req.set_header("Cookie", cookieName + '=' + token);

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID = -1;

        const bool ok = middleware(req, res, &userID);

        ASSERT_TRUE(ok);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
        EXPECT_EQ(userID, expectedUserID);
    }

    TEST(UserIsLoggedIn, IntOverloadPropagatesAuthMissingError)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID = -1;

        const bool ok = middleware(req, res, &userID);

        ASSERT_FALSE(ok);

        // Should be AuthMissing (not overwritten by AuthInvalid)
        EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
        EXPECT_EQ(userID, -1);
    }

    //
    // Int overload: string subject is non-numeric → stoi throws → AuthInvalid
    //

    TEST(UserIsLoggedIn, IntOverloadNonNumericSubjectSetsAuthInvalid)
    {
        httplib::Request  req;
        httplib::Response res;

        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        const std::string cookieName{ Cookies::HostHttpGeckoAuth };
        const std::string subjectStr{ "not-an-int" };

        const auto token =
            jwt::create<JsonTraits>()
                .set_issuer(Issuer::GeckoIssuerName)
                .set_subject(subjectStr)
                .sign(jwt::algorithm::es256{ "", kEcPrivateKey });

        req.set_header("Cookie", cookieName + '=' + token);

        UserIsLoggedIn middleware{ kEcPublicKey };
        int userID = -1;

        const bool ok = middleware(req, res, &userID);

        ASSERT_FALSE(ok);

        // The inner string overload succeeds, but stoi throws → AuthInvalid
        EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
        EXPECT_FALSE(res.body.empty());

        Json::Value  errorJson;
        Json::Reader reader;
        ASSERT_TRUE(reader.parse(res.body, errorJson));
        ASSERT_TRUE(errorJson.isMember("error"));
        EXPECT_EQ(userID, -1);
    }

    //
    // CanBeUsedMultipleTimes sanity check
    //

    TEST(UserIsLoggedIn, CanBeUsedMultipleTimes)
    {
        const std::string cookieName{ Cookies::HostHttpGeckoAuth };

        UserIsLoggedIn middleware{ kEcPublicKey };

        // 1: valid
        {
            httplib::Request  req;
            httplib::Response res;
            int userID;

            res.status = httplib::StatusCode::OK_200;

            const auto token =
                jwt::create<JsonTraits>()
                    .set_issuer(Issuer::GeckoIssuerName)
                    .set_subject("100")
                    .sign(jwt::algorithm::es256{ "", kEcPrivateKey });

            req.set_header("Cookie", cookieName + '=' + token);

            ASSERT_TRUE(middleware(req, res, &userID));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(userID, 100);
        }

        // 2: invalid token
        {
            httplib::Request  req;
            httplib::Response res;
            int userID;

            res.status = httplib::StatusCode::OK_200;

            req.set_header("Cookie", cookieName + "=bad-token");

            ASSERT_FALSE(middleware(req, res, &userID));
            EXPECT_EQ(res.status, httplib::StatusCode::Unauthorized_401);
            EXPECT_FALSE(res.body.empty());
        }

        // 3: valid again
        {
            httplib::Request  req;
            httplib::Response res;
            int userID = -1;

            res.status = httplib::StatusCode::OK_200;

            const auto token =
                jwt::create<JsonTraits>()
                    .set_issuer(Issuer::GeckoIssuerName)
                    .set_subject("7")
                    .sign(jwt::algorithm::es256{ "", kEcPrivateKey });

            req.set_header("Cookie", cookieName + '=' + token);

            ASSERT_TRUE(middleware(req, res, &userID));
            EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
            EXPECT_TRUE(res.body.empty());
            EXPECT_EQ(userID, 7);
        }
    }
}