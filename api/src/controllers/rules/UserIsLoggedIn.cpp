#include "gecko/controllers/rules/UserIsLoggedIn.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/controllers/respond/RespondWithError.h"
#include "gecko/controllers/issuing/Issuer.h"
#include "gecko/controllers/issuing/Cookies.h"
#include "gecko/util/ParseHeader.h"

namespace Gecko::API::Controllers::Rules
{
    bool UserIsLoggedIn::operator()(const httplib::Request& req, httplib::Response& res, std::string* outUserID)
    {
        const auto& cookieHeader = req.get_header_value("Cookie");

        if (!cookieHeader.size())
        {
            Respond::RespondWithError::AuthMissing(res);
            return false;
        }

        const auto cookieValue
            = Util::ParseHeader::GetCookieValue(cookieHeader, Issuing::Cookies::HostHttpGeckoAuth);

        if (!cookieValue)
        {
            Respond::RespondWithError::AuthMissing(res);
            return false;
        }

        try
        {
            const auto decodedAuthCookie 
                = jwt::decode<jwt::traits::open_source_parsers_jsoncpp>(*cookieValue);

            jwt::verify<jwt::traits::open_source_parsers_jsoncpp>()
                    .allow_algorithm(jwt::algorithm::es256(m_pubkey))
                    .with_issuer(Issuing::Issuer::GeckoIssuerName)
                    .verify(decodedAuthCookie);

            *outUserID = decodedAuthCookie.get_subject();
            return true;
        }
        catch (...) { }

        Respond::RespondWithError::AuthInvalid(res);
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

        Respond::RespondWithError::AuthInvalid(res);
        return false;
    }
}