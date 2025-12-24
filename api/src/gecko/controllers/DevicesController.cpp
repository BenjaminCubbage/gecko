#include "gecko/controllers/DevicesController.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/middleware/HasPathParam.h"
#include "gecko/middleware/PathParamEquals.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/models/Device.h"

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     DevicesController::s_jsonReader;
    thread_local Json::FastWriter DevicesController::s_jsonWriter;

    void DevicesController::Attach(httplib::Server& server)
    {
        server.Get(
            "/api/devices/:device_id/status",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_DeviceStatus(req, res);
            });

        server.Get(
            "/api/users/:user_id/devices",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_UsersDevices(req, res);
            });
    }

    void DevicesController::Handle_GET_DeviceStatus(const httplib::Request& req, httplib::Response& res)
    {
        using Services::DevicesService;

        size_t deviceID;
        if (!Middleware::HasPathParam<size_t>{ "device_id" }(req, res, &deviceID))
        {
            std::cout << "blocked invalid request" << std::endl;
            return;
        }

        DevicesService::DeviceStatus status{};
        switch (m_devicesService.GetDeviceStatus(deviceID, &status))
        {
        case DevicesService::Result::Success:
            switch (status)
            {
            case DevicesService::DeviceStatus::Online:
                res.body = R"({"status":"online"})";
                return;

            case DevicesService::DeviceStatus::Offline:
                res.body = R"({"status":"offline"})";
                return;

            default:
                res.body = R"({"status":"pending"})";
                return;
            }

        case DevicesService::Result::DeviceNotFound:
            Http::RespondWithError::DeviceNotFound(res);
            return;

        default:
            Http::RespondWithError::CouldNotFulfill(res);
            return;
        }
    }

    void DevicesController::Handle_GET_UsersDevices(const httplib::Request& req, httplib::Response& res)
    {
        using Services::DevicesService;

        int requesterID{};
        int ownerID{};
        if (
            // !Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &requesterID) ||
            !Middleware::HasPathParam<int>{ "user_id" }(req, res, &ownerID))
        {
            return;
        }

        std::vector<Models::Device> devices{};

        switch (m_devicesService.GetUsersDevices(ownerID, &devices))
        {
            case DevicesService::Result::Success:
            {
                Json::Value response;
                response["devices"] = Json::arrayValue;

                for (int i = 0; i < devices.size(); ++i)
                {
                    response["devices"][i]["device_id"] = devices[i].deviceID;
                    response["devices"][i]["name"]      = devices[i].name;
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