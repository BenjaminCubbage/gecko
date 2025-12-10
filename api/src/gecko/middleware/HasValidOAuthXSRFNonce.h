#pragma once
#include "httplib.h"

namespace Gecko::API::Controllers::Middleware
{
    class HasValidOAuthXSRFNonce
    {
    public:
        bool operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected);
    };
}