#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Middleware
{
    class HasHeader
    {
      public:
        HasHeader(std::string headerName)
            : m_headerName(std::move(headerName)) {}

        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        std::string* outHeaderValue);

      private:
        std::string m_headerName;
    };
}