#pragma once
#include "httplib.h"
#include "json/json.h"
#include "gecko/services/DevicesService.h"
#include "gecko/services/FriendshipsService.h"

namespace Gecko::API::Controllers
{
    class DevicesController
    {
      public:
        DevicesController(Services::DevicesService     devicesService,
                          Services::FriendshipsService friendshipsService,
                          std::string                  pubkey) : 
            m_devicesService(devicesService), 
            m_friendshipsService(friendshipsService),
            m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

      private:
        void Handle_GET_UsersDevices(const httplib::Request& req, httplib::Response& res);

        Services::DevicesService     m_devicesService;
        Services::FriendshipsService m_friendshipsService;
        std::string m_pubkey;

        static thread_local Json::Reader     s_jsonReader;
        static thread_local Json::FastWriter s_jsonWriter;
    };
}