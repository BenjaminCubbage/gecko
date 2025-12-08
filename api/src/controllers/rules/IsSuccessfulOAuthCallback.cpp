#include "gecko/controllers/rules/IsSuccessfulOAuthCallback.h"
#include "gecko/controllers/respond/RespondWithError.h"

namespace Gecko::API::Controllers::Rules
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
            Respond::RespondWithError::OAuthCancelled(res);
            return false;
        }

        *outCode  = req.get_param_value("code");
        *outState = req.get_param_value("state");
        return true;
    }
}