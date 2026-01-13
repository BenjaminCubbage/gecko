#pragma once
#include "httplib.h"

namespace Gecko::API::Middleware
{
    class HasContentLengthLTE
    {
    public:
        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        size_t max,
                        size_t *outContentLength);
    };
}