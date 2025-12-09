#pragma once
#include <string>

namespace Gecko::API::Controllers::Issuing
{
    struct Cookies
    {
        static const std::string HostHttpOAuthXSRFNonce;
        static const std::string HostHttpGeckoAuth;
        static const std::string HostXSRFToken;
    };
}