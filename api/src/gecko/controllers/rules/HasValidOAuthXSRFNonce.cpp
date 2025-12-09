#include "gecko/controllers/rules/HasValidOAuthXSRFNonce.h"
#include "gecko/controllers/respond/RespondWithError.h"
#include "gecko/controllers/issuing/Cookies.h"
#include "gecko/util/UUID.h"
#include "gecko/util/ParseHeader.h"

namespace Gecko::API::Controllers::Rules
{
    bool HasValidOAuthXSRFNonce::operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected)
    {
        const auto cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Respond::RespondWithError::XSRFMissing(res);
            return false;
        }
        
        const auto nonce = Util::ParseHeader::GetCookieValue
        (
            cookieHeader, 
            Issuing::Cookies::HostHttpOAuthXSRFNonce
        );

        if (!nonce)
        {
            Respond::RespondWithError::XSRFMissing(res);
            return false;
        }

        if (*nonce != expected || nonce->size() != Util::UUID::UUIDLength)
        {
            Respond::RespondWithError::XSRFInvalid(res);
            return false;
        }

        return true;
    }
}