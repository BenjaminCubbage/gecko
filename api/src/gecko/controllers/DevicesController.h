#pragma once
#include "httplib.h"
#include "json/json.h"
#include "gecko/services/DevicesService.h"

namespace Gecko::API::Controllers
{
    class DevicesController
    {
    public:
        DevicesController(Services::DevicesService devicesService, std::string pubkey)
            : m_devicesService(devicesService), m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

    private:
        void Handle_GET_DeviceStatus(const httplib::Request& req, httplib::Response& res);

        void Handle_GET_UsersDevices(const httplib::Request& req, httplib::Response& res);

        Services::DevicesService m_devicesService;
        std::string m_pubkey;

        static thread_local Json::Reader     s_jsonReader;
        static thread_local Json::FastWriter s_jsonWriter;
    };
}