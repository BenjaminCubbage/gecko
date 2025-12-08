#include "gecko/controllers/rules/PathParamEquals.h"
#include "gecko/controllers/respond/RespondWithError.h"

namespace Gecko::API::Controllers::Rules
{
    bool PathParamEquals::operator()(const httplib::Request& req, httplib::Response& res, const std::string& expected)
    {
        const auto value = req.path_params.find(m_paramName);

        if (value == req.path_params.end() ||
            value->second != expected)
        {
            Respond::RespondWithError::Forbidden(res);
            return false;
        }

        return true;
    }
}