#include "gecko/controllers/DevicesController.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/middleware/HasPathParam.h"
#include "gecko/middleware/PathParamEquals.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/models/DeviceStatus.h"
#include "gecko/models/DeviceWithStatus.h"

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     DevicesController::s_jsonReader;
    thread_local Json::FastWriter DevicesController::s_jsonWriter;

    void DevicesController::Attach(httplib::Server& server)
    {
        server.Get(
            "/api/users/:user_id/devices",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_UsersDevices(req, res);
            });
    }

    void DevicesController::Handle_GET_UsersDevices(const httplib::Request& req, httplib::Response& res)
    {
        using Services::DevicesService;
        using Services::FriendshipsService;

        int requesterID{};
        int ownerID{};
        if (!Middleware::UserIsLoggedIn{ m_pubkey }(req, res, &requesterID) ||
            !Middleware::HasPathParam<int>{ "user_id" }(req, res, &ownerID))
        {
            return;
        }

        bool areFriends{};
        if (requesterID == ownerID)
        {
            /* Always allow user to access own records. */
            areFriends = true;
        }
        else
        {
            switch (m_friendshipsService.FriendshipExists(requesterID, ownerID, &areFriends))
            {
                case FriendshipsService::Result::OK:
                    break;

                case FriendshipsService::Result::User1NotFound:
                case FriendshipsService::Result::User2NotFound:
                    Http::RespondWithError::UserNotFound(res);
                    return;

                default:
                    Http::RespondWithError::CouldNotFulfill(res);
                    return;
            }
        }

        if (!areFriends)
        {
            Http::RespondWithError::ForbiddenNotFriends(res);
            return;
        }

        std::vector<Models::DeviceWithStatus> devices{};

        switch (m_devicesService.GetUsersDevices(ownerID, &devices))
        {
            case DevicesService::Result::OK:
            {
                Json::Value response;
                response["devices"] = Json::arrayValue;

                for (int i = 0; i < devices.size(); ++i)
                {
                    response["devices"][i]["device_id"] = devices[i].deviceID;
                    response["devices"][i]["name"]      = devices[i].name;

                    switch (devices[i].status)
                    {
                        case Models::DeviceStatus::Online:  response["devices"][i]["status"] = "online";  break;
                        case Models::DeviceStatus::Offline: response["devices"][i]["status"] = "offline"; break;
                        default:                            response["devices"][i]["status"] = "pending"; break;
                    }
                }

                res.body = s_jsonWriter.write(response);
                return;
            }

            case DevicesService::Result::UserNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            case DevicesService::Result::DatabaseError:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
}