#include "gecko/middleware/HasValidOAuthXSRFNonce.h"
#include "gecko/http/Constants.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/util/ParseHeader.h"
#include "gecko/util/UUID.h"

using ::Gecko::API::Http::Constants::Cookies;

namespace Gecko::API::Controllers::Middleware
{
    bool HasValidOAuthXSRFNonce::operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto nonce = Util::ParseHeader::GetCookieValue
        (
            cookieHeader, 
            Cookies::HostHttpOAuthXSRFNonce
        );

        if (!nonce)
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*nonce != expected || nonce->size() != Util::UUID::UUIDLength)
        {
            Http::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}