#pragma once
#include <string>
#include <vector>
#include "httplib.h"
#include "json/json.h"
#include "gecko/services/FriendshipsService.h"

namespace Gecko::API::Controllers
{
    class FriendshipsController
    {
    public:
        FriendshipsController(Services::FriendshipsService friendshipsService,
                              std::string pubkey)
            : m_friendshipsService(std::move(friendshipsService)),
              m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

    private:
        void Handle_GET_Friends(const httplib::Request& req,
                                httplib::Response& res);

        void Handle_GET_FriendRequests(const httplib::Request& req,
                                       httplib::Response& res);

        void Handle_POST_FriendRequests(const httplib::Request& req,
                                        httplib::Response& res);

        void Handle_POST_FriendRequestsAccept(const httplib::Request& req,
                                              httplib::Response& res);

        Services::FriendshipsService m_friendshipsService;
        std::string m_pubkey;

        static thread_local Json::Reader     s_jsonReader;
        static thread_local Json::FastWriter s_jsonWriter;
    };
}