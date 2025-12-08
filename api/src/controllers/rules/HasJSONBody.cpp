#include "gecko/controllers/rules/HasJSONBody.h"
#include "gecko/controllers/respond/RespondWithError.h"

namespace Gecko::API::Controllers::Rules
{
    bool HasJSONBody::operator()(const httplib::Request& req, httplib::Response& res, Json::Value* outParsed)
    {
        thread_local Json::Reader reader;

        if (!reader.parse(req.body, *outParsed))
        {
            Respond::RespondWithError::Unprocessable(res);
            return false;
        }

        return true;
    }
}