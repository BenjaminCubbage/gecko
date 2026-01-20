#include "gecko/middleware/IsSuccessfulOAuthCallback.h"
#include "gecko/http/RespondWithError.h"

namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Middleware
{
    bool IsSuccessfulOAuthCallback::operator()(const httplib::Request& req,
                                               httplib::Response& res,
                                               std::string* outCode,
                                               std::string* outState)
    {
        if (req.has_param("error") ||
            !req.has_param("code") ||
            !req.has_param("state"))
        {
            Http::RespondWithError::OAuthCancelled(res);
            return false;
        }

        *outCode  = req.get_param_value("code");
        *outState = req.get_param_value("state");
        return true;
    }
}