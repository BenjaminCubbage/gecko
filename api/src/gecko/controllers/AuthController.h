#pragma once
#include <chrono>
#include "httplib.h"
#include "json/json.h"
#include "gecko/services/UsersService.h"

namespace Gecko::API::Controllers
{
    class AuthController
    {
      public:
        AuthController(Services::UsersService usersService,
                       std::string domainRoot,
                       std::string backendOrigin,
                       std::string frontendOrigin,
                       std::string oauthClientID,
                       std::string oauthClientSecret,
                       std::string pkey,
                       std::string pubkey) :
            m_usersService(usersService),
            m_domainRoot(std::move(domainRoot)),
            m_backendOrigin(std::move(backendOrigin)),
            m_frontendOrigin(std::move(frontendOrigin)),
            m_oauthClientID(std::move(oauthClientID)),
            m_oauthClientSecret(std::move(oauthClientSecret)),
            m_pkey(std::move(pkey)),
            m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

      private:
        void Handle_GET_LogIn(const httplib::Request& req, httplib::Response& res);
        void Handle_POST_LogOut(const httplib::Request& req, httplib::Response& res);
        void Handle_GET_OAuthCallback(const httplib::Request& req, httplib::Response& res);
        void Handle_POST_Refresh(const httplib::Request& req, httplib::Response& res);
        void Handle_GET_XSRF(const httplib::Request& req, httplib::Response& res);

        Services::UsersService m_usersService;

        const std::string m_domainRoot;
        const std::string m_backendOrigin;
        const std::string m_frontendOrigin;

        const std::string m_oauthClientID;
        const std::string m_oauthClientSecret;

        const std::string m_pkey;
        const std::string m_pubkey;

        // These are reusable but thread-unsafe.
        static thread_local Json::Reader     s_jsonReader;
        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local httplib::Client  s_accountsGoogleComClient;
    };
}