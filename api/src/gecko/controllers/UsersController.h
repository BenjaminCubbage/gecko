#pragma once
#include <string>
#include "httplib.h"
#include "json/json.h"
#include "gecko/services/UsersService.h"

namespace Gecko::API::Controllers
{
    class UsersController
    {
    public:
        UsersController(Services::UsersService usersService, std::string pubkey) :
                        m_usersService(usersService),
                        m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

    private:
        void Handle_GET_Me(const httplib::Request& req, httplib::Response& res);
        void Handle_PATCH_User(const httplib::Request& req, httplib::Response& res);

        Services::UsersService m_usersService;
        std::string m_pubkey;
        
        // note(ben): These are reusable but not thread-unsafe.
        static thread_local Json::Reader     s_jsonReader;
        static thread_local Json::FastWriter s_jsonWriter;
    };
}