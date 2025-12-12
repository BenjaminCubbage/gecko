#include "gecko/middleware/HasValidOAuthXSRFNonce.h"
#include "gecko/http/Constants.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/http/ParseHeader.h"
#include "gecko/http/UUID.h"

using ::Gecko::API::Http::Constants::Cookies;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Middleware
{
    bool HasValidOAuthXSRFNonce::operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto nonce = Http::ParseHeader::GetCookieValue
        (
            cookieHeader, 
            Cookies::HostHttpOAuthXSRFNonce
        );

        if (!nonce)
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*nonce != expected || nonce->size() !=  Http::UUID::UUIDLength)
        {
            Http::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}