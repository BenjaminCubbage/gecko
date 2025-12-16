#pragma once
#include "httplib.h"
#include "gecko/services/DevicesService.h"

namespace Gecko::API::Controllers
{
    class DevicesController
    {
    public:
        DevicesController(Services::DevicesService devicesService)
            : m_devicesService(devicesService) {}

        void Attach(httplib::Server& server);

    private:
        void Handle_GET_DeviceStatus(const httplib::Request& req, httplib::Response& res);

        Services::DevicesService m_devicesService;
    };
}