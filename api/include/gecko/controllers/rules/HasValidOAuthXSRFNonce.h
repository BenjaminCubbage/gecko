#pragma once
#include "httplib.h"

namespace Gecko::API::Controllers::Rules
{
    class HasValidOAuthXSRFNonce
    {
    public:
        bool operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected);
    };
}