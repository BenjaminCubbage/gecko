#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/http/Constants.h"
#include "gecko/util/ParseHeader.h"
#include "gecko/util/UUID.h"

using ::Gecko::API::Http::Constants::Cookies;
using ::Gecko::API::Http::Constants::Headers;

namespace Gecko::API::Controllers::Middleware
{
    bool HasValidXSRFToken::operator()(const httplib::Request& req, httplib::Response& res)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto cookieToken = Util::ParseHeader::GetCookieValue
        (
            cookieHeader,
            Cookies::HostXSRFToken
        );

        if (!cookieToken)
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        const auto headerToken = req.get_header_value(Headers::XXSRFToken);

        if (!headerToken.size())
        {
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*cookieToken != headerToken || headerToken.size() != Util::UUID::UUIDLength)
        {
            Http::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}