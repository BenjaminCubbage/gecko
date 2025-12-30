#include "gecko/controllers/FriendshipsController.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/middleware/HasJSONBody.h"
#include "gecko/middleware/HasJSONValueMember.h"
#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/middleware/PathParamEquals.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/models/User.h"

namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     FriendshipsController::s_jsonReader;
    thread_local Json::FastWriter FriendshipsController::s_jsonWriter;

    void FriendshipsController::Attach(httplib::Server& server)
    {
        server.Get(
            "/api/users/:id/friends",
            [this](const httplib::Request& req, httplib::Response& res) {
                Handle_GET_Friends(req, res);
            });

        server.Get(
            "/api/users/:id/friend-requests",
            [this](const httplib::Request& req, httplib::Response& res) {
                Handle_GET_FriendRequests(req, res);
            });

        server.Post(
            "/api/users/:id/friend-requests",
            [this](const httplib::Request& req, httplib::Response& res) {
                Handle_POST_FriendRequests(req, res);
            });

        server.Post(
            "/api/users/:id/friend-requests/accept",
            [this](const httplib::Request& req, httplib::Response& res) {
                Handle_POST_FriendRequestsAccept(req, res);
            });
    }

    void FriendshipsController::Handle_GET_Friends(const httplib::Request& req,
                                                   httplib::Response& res)
    {
        using Services::FriendshipsService;

        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        std::vector<Models::User> friends{};
        switch (m_friendshipsService.GetFriendships(userID, &friends))
        {
            case FriendshipsService::Result::Success:
            {
                Json::Value response{ Json::objectValue };
                response["friends"] = Json::arrayValue;

                for (const auto& friendUser : friends)
                {
                    Json::Value item{ Json::objectValue };
                    item["user_id"]  = friendUser.userID;
                    item["username"] = friendUser.username;
                    response["friends"].append(std::move(item));
                }

                res.body = s_jsonWriter.write(response);
                return;
            }

            case FriendshipsService::Result::UserNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }

    void FriendshipsController::Handle_GET_FriendRequests(const httplib::Request& req,
                                                          httplib::Response& res)
    {
        using Services::FriendshipsService;

        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        std::vector<Models::User> incoming{};
        std::vector<Models::User> outgoing{};

        switch (m_friendshipsService.GetFriendRequests(userID, &incoming, &outgoing))
        {
            case FriendshipsService::Result::Success:
            {
                Json::Value response{ Json::objectValue };
                response["friend_requests"] = Json::objectValue;
                response["friend_requests"]["outgoing"] = Json::arrayValue;
                response["friend_requests"]["incoming"] = Json::arrayValue;

                for (const auto& u : outgoing)
                {
                    Json::Value item{ Json::objectValue };
                    item["status"] = "pending";
                    item["user"]["user_id"]  = u.userID;
                    item["user"]["username"] = u.username;
                    response["friend_requests"]["outgoing"].append(std::move(item));
                }

                for (const auto& u : incoming)
                {
                    Json::Value item{ Json::objectValue };
                    item["status"] = "pending";
                    item["user"]["user_id"]  = u.userID;
                    item["user"]["username"] = u.username;
                    response["friend_requests"]["incoming"].append(std::move(item));
                }

                res.body = s_jsonWriter.write(response);
                return;
            }

            case FriendshipsService::Result::UserNotFound:
            case FriendshipsService::Result::User1NotFound:
            case FriendshipsService::Result::User2NotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
        
    void FriendshipsController::Handle_POST_FriendRequests(const httplib::Request& req,
                                                            httplib::Response& res)
    {
        using Services::FriendshipsService;

        int userID{};
        int otherUserID{};
        Json::Value body;

        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::HasValidXSRFToken{}(req, res) ||
            !Middleware::HasJSONBody{}(req, res, &body) ||
            !Middleware::HasJSONValueMember<int>{ "user_id" }(req, res, body, &otherUserID) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        switch (m_friendshipsService.CreateFriendRequest(userID, otherUserID))
        {
            case FriendshipsService::Result::Success:
                res.status = httplib::StatusCode::Created_201;
                return;

            // Idempotent semantics
            case FriendshipsService::Result::AlreadyFriends:
            case FriendshipsService::Result::FriendRequestAlreadyExists:
                res.status = httplib::StatusCode::OK_200;
                return;

            case FriendshipsService::Result::SelfFriendNotAllowed:
                Http::RespondWithError::SelfFriendNotAllowed(res);
                return;

            case FriendshipsService::Result::UserNotFound:
            case FriendshipsService::Result::User1NotFound:
            case FriendshipsService::Result::User2NotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }

    void FriendshipsController::Handle_POST_FriendRequestsAccept(const httplib::Request& req,
                                                                httplib::Response& res)
    {
        using Services::FriendshipsService;

        int userID{};
        int otherUserID{};
        Json::Value body;

        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)) ||
            !Middleware::HasValidXSRFToken{ }(req, res) ||
            !Middleware::HasJSONBody{ }(req, res, &body) ||
            !Middleware::HasJSONValueMember<int>{ "user_id" }(req, res, body, &otherUserID))
        {
            return;
        }

        switch (m_friendshipsService.AcceptFriendRequest(userID, otherUserID))
        {
            case FriendshipsService::Result::Success:
            case FriendshipsService::Result::AlreadyFriends:
                res.status = httplib::StatusCode::OK_200;
                return;

            case FriendshipsService::Result::SelfFriendNotAllowed:
                Http::RespondWithError::SelfFriendNotAllowed(res);
                return;

            case FriendshipsService::Result::FriendRequestNotFound:
                Http::RespondWithError::FriendRequestNotFound(res);
                return;

            case FriendshipsService::Result::UserNotFound:
            case FriendshipsService::Result::User1NotFound:
            case FriendshipsService::Result::User2NotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
}