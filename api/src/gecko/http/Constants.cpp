#include "gecko/http/Constants.h"

namespace Gecko::API::Http::Constants
{
    const std::string Issuer::GeckoIssuerName        { "gecko" };
    const std::string Headers::XXSRFToken            { "X-XSRF-TOKEN" };
    const std::string Headers::IdempotencyKey        { "Idempotency-Key" };
    const std::string Cookies::HostHttpOAuthXSRFNonce{ "__Host-Http-oauth_xsrf_nonce" };
    const std::string Cookies::HostHttpGeckoAuth     { "__Host-Http-gecko_auth" };
    const std::string Cookies::HostXSRFToken         { "xsrf_token" };
}