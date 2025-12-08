#pragma once
#include <string>
#include "httplib.h"
#include "gecko/controllers/issuing/Cookies.h"

namespace Gecko::API::Controllers::Rules
{
    class HasValidXSRFToken
    {
    public:
        bool operator()(const httplib::Request& req, httplib::Response& res);
    };
}