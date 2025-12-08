#include <iostream>
#include "gecko/controllers/UsersController.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/controllers/respond/RespondWithError.h"
#include "gecko/models/User.h"
#include "gecko/models/UserPatch.h"
#include "gecko/controllers/rules/HasJSONBody.h"
#include "gecko/controllers/rules/HasJSONValueMember.h"
#include "gecko/controllers/rules/UserIsLoggedIn.h"
#include "gecko/controllers/rules/PathParamEquals.h"
#include "gecko/controllers/respond/RespondWithError.h"

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     UsersController::s_jsonReader{};
    thread_local Json::FastWriter UsersController::s_jsonWriter{};

    
    void UsersController::Attach(httplib::Server& server)
    {
        server.Get("/api/users/me", [this] (const httplib::Request& req, httplib::Response& res) {
            Handle_GET_Me(req, res);
        });

        server.Patch("/api/users/:id", [this] (const httplib::Request& req, httplib::Response& res) {
            Handle_PATCH_User(req, res);
        });
    }

    void UsersController::Handle_GET_Me(const httplib::Request& req, httplib::Response& res)
    {
        using Services::UsersService;

        int userID{};
        if (!Rules::UserIsLoggedIn{ m_pubkey }(req, res, &userID))
            return;

        Models::User user;
        switch (m_usersService.GetUser(userID, &user))
        {
            case UsersService::Result::Success:
            {
                Json::Value response{ };
                response["user"] = Json::Value{ };
                response["user"]["user_id"]  = user.userID;
                response["user"]["username"] = user.username;
                res.body = s_jsonWriter.write(response);
                return;
            }

            case UsersService::Result::UserNotFound:
                Respond::RespondWithError::UserNotFound(res);
                return;

            default:
                Respond::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }

    void UsersController::Handle_PATCH_User(const httplib::Request& req, httplib::Response& res)
    {
        using Services::UsersService;

        int userID{};
        Json::Value patch;
        std::string username;

        if (!Rules::UserIsLoggedIn{ m_pubkey }(req, res, &userID) ||
            !Rules::HasJSONBody{ }(req, res, &patch) ||
            !Rules::HasJSONValueMember<std::string>{ "username" }(req, res, patch, &username) ||
            !Rules::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        switch (m_usersService.PatchUser(userID, { .username = username }))
        {
            case UsersService::Result::Success:
                return;

            case UsersService::Result::UserNotFound:
                Respond::RespondWithError::UserNotFound(res);
                return;

            case UsersService::Result::UsernameTooLong:
                Respond::RespondWithError::UsernameTooLong(res);
                return;

            case UsersService::Result::UsernameTooShort:
                Respond::RespondWithError::UsernameTooShort(res);
                return;

            case UsersService::Result::UsernameTaken:
                Respond::RespondWithError::UsernameTaken(res);
                return;

            default:
                Respond::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
}