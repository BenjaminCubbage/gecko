#include "gecko/controllers/rules/HasValidXSRFToken.h"
#include "gecko/controllers/respond/RespondWithError.h"
#include "gecko/controllers/issuing/Cookies.h"
#include "gecko/controllers/issuing/Headers.h"
#include "gecko/util/ParseHeader.h"
#include "gecko/util/UUID.h"

namespace Gecko::API::Controllers::Rules
{
    bool HasValidXSRFToken::operator()(const httplib::Request& req, httplib::Response& res)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Respond::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto cookieToken = Util::ParseHeader::GetCookieValue
        (
            cookieHeader,
            Issuing::Cookies::HostXSRFToken
        );

        if (!cookieToken)
        {
            Respond::RespondWithError::XSRFMissing(res);
            return false;
        }

        const auto headerToken = req.get_header_value(Issuing::Headers::XXSRFToken);

        if (!headerToken.size())
        {
            Respond::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*cookieToken != headerToken || headerToken.size() != Util::UUID::UUIDLength)
        {
            Respond::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}