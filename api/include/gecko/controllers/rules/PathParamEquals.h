#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Controllers::Rules
{
    class PathParamEquals
    {
    public:
        PathParamEquals(std::string paramName)
            : m_paramName(std::move(paramName)) {}
        bool operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected);
        const std::string m_paramName;
    };
}