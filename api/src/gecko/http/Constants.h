#pragma once
#include <string>

namespace Gecko::API::Http::Constants
{
    struct Issuer
    {
        static const std::string GeckoIssuerName;
    };

    struct Headers
    {
        static const std::string XXSRFToken;
    };

    struct Cookies
    {
        static const std::string HostHttpOAuthXSRFNonce;
        static const std::string HostHttpGeckoAuth;
        static const std::string HostXSRFToken;
    };
}