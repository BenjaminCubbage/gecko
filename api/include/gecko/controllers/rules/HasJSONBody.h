#pragma once
#include <string>
#include "httplib.h"
#include "json/json.h"

namespace Gecko::API::Controllers::Rules
{
    class HasJSONBody
    {
    public:
        bool operator()(const httplib::Request& req, httplib::Response& res, Json::Value* outParsed);
        const std::string m_pubkey;
    };
}