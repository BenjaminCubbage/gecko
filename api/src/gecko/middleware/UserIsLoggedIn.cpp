#include "gecko/middleware/UserIsLoggedIn.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/http/Constants.h"
#include "gecko/http/RespondWithError.h"
#include "gecko/util/ParseHeader.h"

using ::Gecko::API::Http::Constants::Issuer;
using ::Gecko::API::Http::Constants::Cookies;

namespace Gecko::API::Controllers::Middleware
{
    bool UserIsLoggedIn::operator()(const httplib::Request& req, httplib::Response& res, std::string* outUserID)
    {
        const auto& cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Http::RespondWithError::AuthMissing(res);
            return false;
        }

        const auto cookieValue
            = Util::ParseHeader::GetCookieValue(cookieHeader, Cookies::HostHttpGeckoAuth);

        if (!cookieValue)
        {
            Http::RespondWithError::AuthMissing(res);
            return false;
        }

        try
        {
            const auto decodedAuthCookie 
                = jwt::decode<jwt::traits::open_source_parsers_jsoncpp>(*cookieValue);

            jwt::verify<jwt::traits::open_source_parsers_jsoncpp>()
                    .allow_algorithm(jwt::algorithm::es256(m_pubkey))
                    .with_issuer(Issuer::GeckoIssuerName)
                    .verify(decodedAuthCookie);

            *outUserID = decodedAuthCookie.get_subject();
            return true;
        }
        catch (...) { }

        Http::RespondWithError::AuthInvalid(res);
        return false;
    }

    bool UserIsLoggedIn::operator()(const httplib::Request& req, httplib::Response& res, int* outUserID)
    {
        try
        {
            std::string userIDStr;
            bool result = this->operator()(req, res, &userIDStr);
            
            if (result)
                *outUserID = std::stoi(userIDStr);

            return result;
        }
        catch (...) { }

        Http::RespondWithError::AuthInvalid(res);
        return false;
    }
}