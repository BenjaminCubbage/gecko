#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/http/Constants.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/http/ParseHeader.h"
#include "gecko/http/UUID.h"

using ::Gecko::API::Http::Constants::Cookies;
using ::Gecko::API::Http::Constants::Headers;
namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Middleware
{
    bool HasValidXSRFToken::operator()(const httplib::Request& req, httplib::Response& res)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Gecko::API::Http::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto cookieToken = Gecko::API::Http::ParseHeader::GetCookieValue
        (
            cookieHeader,
            Cookies::HostXSRFToken
        );

        if (!cookieToken)
        {
            Gecko::API::Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        const auto headerToken = req.get_header_value(Headers::XXSRFToken);

        if (!headerToken.size())
        {
            Gecko::API::Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*cookieToken != headerToken || headerToken.size() != Http::UUID::UUIDLength)
        {
            Gecko::API::Http::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}