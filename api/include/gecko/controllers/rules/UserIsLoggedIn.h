#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Controllers::Rules
{
    class UserIsLoggedIn
    {
    public:
        UserIsLoggedIn(std::string pubkey)
            : m_pubkey(std::move(pubkey)) {}
        bool operator()(const httplib::Request& req, httplib::Response& res, std::string* outUserID);
        bool operator()(const httplib::Request& req, httplib::Response& res, int* outUserID);
        const std::string m_pubkey;
    };
}