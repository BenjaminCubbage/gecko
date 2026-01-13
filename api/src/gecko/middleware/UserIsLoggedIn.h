#pragma once
#include <string>
#include <type_traits>
#include "httplib.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/http/Constants.h"
#include "gecko/http/ParseHeader.h"
#include "gecko/http/ParseValue.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    class UserIsLoggedIn
    {
    public:
        UserIsLoggedIn(std::string pubkey) 
            : m_pubkey(std::move(pubkey)) {}

        bool operator()(const httplib::Request& req, httplib::Response& res, int* outUserID)
        {
            const auto& cookieHeader = req.get_header_value("Cookie");

            if (!cookieHeader.size())
            {
                Http::RespondWithError::AuthMissing(res);
                return false;
            }

            const auto cookieValue
                = Http::ParseHeader::GetCookieValue(cookieHeader, Http::Constants::Cookies::HostHttpGeckoAuth);

            if (!cookieValue)
            {
                Http::RespondWithError::AuthMissing(res);
                return false;
            }

            try
            {
                const auto decodedAuthCookie 
                    = jwt::decode<jwt::traits::open_source_parsers_jsoncpp>(*cookieValue);
                    
                if (!Http::ParseValue::TryParseIntegral<int>(decodedAuthCookie.get_subject(), outUserID))
                {
                    Http::RespondWithError::AuthInvalid(res);
                    return false;
                }

                jwt::verify<jwt::traits::open_source_parsers_jsoncpp>()
                        .allow_algorithm(jwt::algorithm::es256(m_pubkey))
                        .with_issuer(Http::Constants::Issuer::GeckoIssuerName)
                        .verify(decodedAuthCookie);

                return true;
            }
            catch (...) { }

            Http::RespondWithError::AuthInvalid(res);
            return false;
        }

    private:
        const std::string m_pubkey;
    };
}