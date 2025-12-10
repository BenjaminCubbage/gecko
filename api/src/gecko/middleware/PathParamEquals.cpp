#include "gecko/middleware/PathParamEquals.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Controllers::Middleware
{
    bool PathParamEquals::operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected)
    {
        const auto value = req.path_params.find(m_paramName);

        if (value == req.path_params.end() ||
            value->second != expected)
        {
            Http::RespondWithError::Forbidden(res);
            return false;
        }

        return true;
    }
}