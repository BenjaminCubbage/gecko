#include "gecko/controllers/DevicesController.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Controllers
{
    void DevicesController::Attach(httplib::Server& server)
    {
        server.Get(
            "/api/devices/:device_id/status",
            [this] (const httplib::Request& req, httplib::Response& res) {
                Handle_GET_DeviceStatus(req, res);
            });
    }

    void DevicesController::Handle_GET_DeviceStatus(const httplib::Request& req, httplib::Response& res)
    {
        using Services::DevicesService;

        // todo(ben): validate

        int deviceID = std::stoi(req.path_params.at("device_id"));

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
                res.status = httplib::StatusCode::Accepted_202;
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
}