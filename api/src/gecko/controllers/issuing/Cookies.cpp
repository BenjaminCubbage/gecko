#include "gecko/controllers/issuing/Cookies.h"

namespace Gecko::API::Controllers::Issuing
{
    const std::string Cookies::HostHttpOAuthXSRFNonce{ "__Host-Http-oauth_xsrf_nonce" };
    const std::string Cookies::HostHttpGeckoAuth{ "__Host-Http-gecko_auth" };
    const std::string Cookies::HostXSRFToken{ "__Host-xsrf_token" };
}