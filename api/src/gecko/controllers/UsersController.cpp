#include <iostream>
#include "gecko/controllers/UsersController.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/middleware/HasJSONBody.h"
#include "gecko/middleware/HasJSONValueMember.h"
#include "gecko/middleware/HasQueryParam.h"
#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/middleware/PathParamEquals.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/models/User.h"
#include "gecko/models/UserPatch.h"

namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     UsersController::s_jsonReader{};
    thread_local Json::FastWriter UsersController::s_jsonWriter{};

    void UsersController::Attach(httplib::Server& server)
    {
        server.Get(
            "/api/users/me",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_Me(req, res);
            });

        server.Patch(
            "/api/users/:id",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_PATCH_User(req, res);
            });

        server.Get(
            "/api/users",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_UserQuery(req, res);
            });
    }

    void UsersController::Handle_GET_Me(const httplib::Request& req, httplib::Response& res)
    {
        using Services::UsersService;

        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID))
            return;

        Models::User user;
        switch (m_usersService.GetUser(userID, &user))
        {
            case UsersService::Result::OK:
            {
                Json::Value response{ };
                response["user"] = Json::Value{ };
                response["user"]["user_id"]  = user.userID;
                response["user"]["username"] = user.username;
                res.body = s_jsonWriter.write(response);
                return;
            }

            case UsersService::Result::UserNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }

    void UsersController::Handle_PATCH_User(const httplib::Request& req, httplib::Response& res)
    {
        using Services::UsersService;

        int userID{};
        Json::Value patch;
        std::string username;

        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::HasValidXSRFToken{ }(req, res) ||
            !Middleware::HasJSONBody{ }(req, res, &patch) ||
            !Middleware::HasJSONValueMember<std::string>{ "username" }(req, res, patch, &username) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        switch (m_usersService.PatchUser(userID, { .username = username }))
        {
            case UsersService::Result::OK:
                return;

            case UsersService::Result::UserNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            case UsersService::Result::UsernameTooLong:
                Http::RespondWithError::UsernameTooLong(res);
                return;

            case UsersService::Result::UsernameTooShort:
                Http::RespondWithError::UsernameTooShort(res);
                return;

            case UsersService::Result::UsernameContainsInvalidCharacters:
                Http::RespondWithError::UsernameContainsInvalidCharacters(res);
                return;

            case UsersService::Result::UsernameTaken:
                Http::RespondWithError::UsernameTaken(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
    
    void UsersController::Handle_GET_UserQuery(const httplib::Request& req, httplib::Response& res)
    {
        using Services::UsersService;

        int userID{};
        Json::Value patch;
        std::string username;

        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::HasQueryParam<std::string>{ "username" }(req, res, &username))
        {
            return;
        }

        Models::User user;

        switch (m_usersService.GetUserByUsername(username, &user))
        {
            case UsersService::Result::OK:
            {
                Json::Value response{ };
                response["user"] = Json::Value{ };
                response["user"]["user_id"]  = user.userID;
                response["user"]["username"] = user.username;
                res.body = s_jsonWriter.write(response);
                return;
            }

            case UsersService::Result::UsernameTooLong:
                Http::RespondWithError::UsernameTooLong(res);
                return;

            case UsersService::Result::UsernameTooShort:
                Http::RespondWithError::UsernameTooShort(res);
                return; 

            case UsersService::Result::UsernameContainsInvalidCharacters:
                Http::RespondWithError::UsernameContainsInvalidCharacters(res);
                return;

            case UsersService::Result::UserNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
            case UsersService::Result::DatabaseError:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
}