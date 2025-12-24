#include "gecko/controllers/AuthController.h"
#include <format>
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/http/Constants.h"
#include "gecko/http/ParseHeader.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/middleware/HasValidOAuthXSRFNonce.h"
#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/middleware/IsSuccessfulOAuthCallback.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/models/User.h"
#include "gecko/rand/UUID.h"

// macros instead of constexpr for nicer concatenation
#define ONE_WEEK_IN_SECONDS      604800
#define ONE_WEEK_IN_SECONDS_STR "604800"

using ::Gecko::API::Http::Constants::Cookies;
using ::Gecko::API::Http::Constants::Issuer;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     AuthController::s_jsonReader{};
    thread_local Json::FastWriter AuthController::s_jsonWriter{};
    thread_local httplib::Client  AuthController::s_accountsGoogleComClient{ "https://accounts.google.com" };


    void AuthController::Attach(httplib::Server& server)
    {
        server.Get("/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
            Handle_GET_LogIn(req, res);
        });

        server.Post("/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
            Handle_POST_LogOut(req, res);
        });

        server.Get("/auth/oauth-callback", [this](const httplib::Request& req, httplib::Response& res) {
            Handle_GET_OAuthCallback(req, res);
        });

        server.Post("/auth/refresh", [this](const httplib::Request& req, httplib::Response& res) {
            Handle_POST_Refresh(req, res);
        });

        server.Get("/auth/xsrf", [this](const httplib::Request& req, httplib::Response& res) {
            Handle_GET_XSRF(req, res);
        });
    }


    void AuthController::Handle_GET_LogIn(const httplib::Request& req, httplib::Response& res)
    {
        const std::string nonce = Rand::UUID::GenerateUUID();

        const std::string uri = std::format
        (
            "https://accounts.google.com/o/oauth2/v2/auth"
            "?response_type=code"
            "&redirect_uri=https://localhost:3000/auth/oauth-callback"
            "&client_id={}"
            "&scope=openid"
            "&state={}"
            ,
            m_oauthClientID,
            nonce
        );

        const auto nonceCookie = std::format
        (
            "{}={};"
            "HTTPOnly;"
            "Secure;"
            "Max-Age=900;"
            "SameSite=Lax;"
            "Path=/;"
            ,
            Cookies::HostHttpOAuthXSRFNonce,
            nonce
        );

        res.set_header("Set-Cookie", nonceCookie);
        res.set_redirect(httplib::encode_uri(uri));
    }


    void AuthController::Handle_POST_LogOut(const httplib::Request& req, httplib::Response& res)
    {
        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::HasValidXSRFToken{ }(req, res))
        {
            return;
        }

        const auto expireCookie = std::format
        (
            "{}=;"
            "HTTPOnly;"
            "Secure;"
            "SameSite=Strict;"
            "Expires=Thu, 01 Jan 1970 00:00:00 GMT;"
            "Path=/;"
            ,
            Cookies::HostHttpGeckoAuth
        );

        res.set_header("Set-Cookie", expireCookie);
    }


    void AuthController::Handle_GET_OAuthCallback(const httplib::Request &req, httplib::Response &res)
    {
        std::string code;
        std::string state;

        if (!Middleware::IsSuccessfulOAuthCallback{ }(req, res, &code, &state) ||
            !Middleware::HasValidOAuthXSRFNonce{ }(req, res, state))
        {
            res.set_redirect("/");
            return;
        }

        // This is blocking, and the default thread pool size of cpphttplib is 8, so
        // in theory this could stall if a ton of requests came in. If this were to
        // happen, I would need to dynamically size the httplib thread pool count to
        // scale up the threads when too many questions start coming in. This is
        // indeed something cpphttplib supports.

        // However, I don't think this will be a problem for the two people who will
        // end up using this application :^)

        const auto codeRequestResponse = s_accountsGoogleComClient.Post
        (
            // Not encoding the uri seems to be fine here.
            std::format
            (
                "/o/oauth2/token"
                "?grant_type=authorization_code"
                "&redirect_uri=https://localhost:3000/auth/oauth-callback"
                "&client_id={}"
                "&client_secret={}"
                "&code={}"
                ,
                m_oauthClientID,
                m_oauthClientSecret,
                code
            )
        );

        // Parse JSON out of response to get id_token.

        // We could double-check scope here, but since we only asked for the openid
        // scope and it's returned success, it wouldn't have much benefit.

        Json::Value codeRequestResponseJson;
        if (!codeRequestResponse ||
            !s_jsonReader.parse(codeRequestResponse->body, codeRequestResponseJson) ||
            !codeRequestResponseJson["id_token"].isString())
        {
            Http::RespondWithError::OAuthInternalError(res);
            return;
        }

        try
        {
            // We aren't verifying the decoded id_token's signature here, because we already know this
            // comes from google.com.
            const auto decodedIDToken
                = jwt::decode<jwt::traits::open_source_parsers_jsoncpp>(codeRequestResponseJson["id_token"].asString());

            const auto iss = decodedIDToken.get_issuer();
            const auto sub = decodedIDToken.get_subject();

            if (iss != "accounts.google.com" || !sub.size())
            {
                // Should never happen
                Http::RespondWithError::OAuthInternalError(res);
                return;
            }

            const std::string username = "user_" + Gecko::API::Rand::UUID::GenerateUUID().substr(0, 8);

            switch (m_usersService.CreateUser(username, iss, sub))
            {
                case Services::UsersService::Result::Success:
                case Services::UsersService::Result::UserAlreadyExists:
                    break;

                default:
                    Http::RespondWithError::CouldNotFulfill(res);
                    return;
            }

            int userID{};
            if (m_usersService.GetUserIDByOIDC(iss, sub, &userID) != Services::UsersService::Result::Success)
            {
                // User _should_ exist by now.
                Http::RespondWithError::CouldNotFulfill(res);
                return;
            }

            // User's authorization cookie
            // Its "iss" is now us, and the "sub" is our internal user_id
            const auto authCookie = std::format
            (
                "{}={};"
                "HTTPOnly;"
                "Secure;"
                "SameSite=Strict;"
                "Max-Age=" ONE_WEEK_IN_SECONDS_STR ";"
                "Path=/;"
                ,
                Cookies::HostHttpGeckoAuth,
                jwt::create<jwt::traits::open_source_parsers_jsoncpp>()
                    .set_type("JWT")
                    .set_issuer(Issuer::GeckoIssuerName)
                    .set_issued_now()
                    .set_expires_in(std::chrono::seconds{ ONE_WEEK_IN_SECONDS })
                    .set_subject(std::to_string(userID))
                    .sign(jwt::algorithm::es256("", m_pkey))
            );

            res.set_header("Set-Cookie", authCookie);
            res.set_redirect("/");
            return;
        }
        catch (...)
        {
            Http::RespondWithError::OAuthInternalError(res);
            return;
        }
    }


    void AuthController::Handle_POST_Refresh(const httplib::Request &req, httplib::Response &res)
    {
        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID))
            return;

        try
        {
            const auto refreshedCookie = std::format
            (
                "{}={};"
                "HTTPOnly;"
                "Secure;"
                "SameSite=Strict;"
                "Max-Age=" ONE_WEEK_IN_SECONDS_STR ";"
                "Path=/;"
                ,
                Cookies::HostHttpGeckoAuth,
                jwt::create<jwt::traits::open_source_parsers_jsoncpp>()
                    .set_type("JWT")
                    .set_issuer(Issuer::GeckoIssuerName)
                    .set_subject(std::to_string(userID))
                    .set_expires_in(std::chrono::seconds{ ONE_WEEK_IN_SECONDS })
                    .set_issued_now()
                    .sign(jwt::algorithm::es256("", m_pkey))
            );

            res.set_header("Set-Cookie", refreshedCookie);
            return;
        }
        catch (...)
        {
            Http::RespondWithError::AuthInvalid(res);
            return;
        }
    }

    void AuthController::Handle_GET_XSRF(const httplib::Request& req, httplib::Response& res)
    {
        const auto cookie = std::format
        (
            "{}={};"
            "Secure;"
            "Max-Age=" ONE_WEEK_IN_SECONDS_STR ";"
            "SameSite=Strict;"
            "Path=/;"
            ,
            Cookies::HostXSRFToken,
            Rand::UUID::GenerateUUID()
        );

        res.set_header("Set-Cookie", cookie);
    }
}