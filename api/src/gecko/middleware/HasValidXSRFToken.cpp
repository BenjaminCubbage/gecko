#include "gecko/middleware/HasValidXSRFToken.h"
#include "gecko/http/Constants.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/http/ParseHeader.h"
#include "gecko/rand/UUID.h"

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
            Http::RespondWithError::XSRFMissing(res);
            return false;
        }

        const auto cookieToken = Http::ParseHeader::GetCookieValue
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

        if (*cookieToken != headerToken || headerToken.size() != Rand::UUID::UUIDLength)
        {
            Http::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}