#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Controllers::Middleware
{
    class HasValidXSRFToken
    {
    public:
        bool operator()(const httplib::Request& req, httplib::Response& res);
    };
}