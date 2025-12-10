#include "gecko/middleware/HasJSONBody.h"
#include "gecko/http/RespondWithError.h"

namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Middleware
{
    bool HasJSONBody::operator()(const httplib::Request& req, httplib::Response& res, Json::Value* outParsed)
    {
        thread_local Json::Reader reader;

        if (!reader.parse(req.body, *outParsed))
        {
            Http::RespondWithError::Unprocessable(res);
            return false;
        }

        return true;
    }
}