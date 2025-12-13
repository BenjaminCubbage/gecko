#pragma once
#include "httplib.h"

namespace Gecko::API::Middleware
{
    class HasContentLength
    {
    public:
        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        size_t *outContentLength);
    };
}