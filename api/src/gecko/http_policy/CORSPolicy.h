#pragma once
#include <format>
#include <string>
#include <utility>
#include "httplib.h"
#include "gecko/http/Constants.h"

namespace Gecko::API::HttpPolicy
{
    class CORSPolicy
    {
        using Headers = Http::Constants::Headers;

      private:
        static inline const std::string AllowHeaders
        {
            std::format
            (
                "Accept, Content-Type, Origin, {}, {}"
                ,
                Headers::XXSRFToken,
                Headers::IdempotencyKey
            )
        };

        static inline const std::string AllowCredentials
        {
            "true"
        };

        static inline const std::string AllowMethods
        {
            "PUT, PATCH, DELETE"
        };

      public:
        CORSPolicy(std::string allowList) :
            m_allowList(std::move(allowList)) {}

        void Attach(httplib::Server& server);

      private:
        std::string m_allowList;
    };
}