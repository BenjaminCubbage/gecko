#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Middleware
{
    class IsSuccessfulOAuthCallback
    {
    public:
        bool operator()(const httplib::Request& req, 
                        httplib::Response& res, 
                        std::string* outCode, 
                        std::string* outState);
    };
}