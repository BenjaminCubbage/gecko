#include "gecko/middleware/HasContentLengthLTE.h"
#include "gecko/http/ParseValue.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    bool HasContentLengthLTE::operator()(const httplib::Request& req,
                                        httplib::Response& res,
                                        size_t max,
                                        size_t *outContentLength)
    {
        const auto contentLength = req.get_header_value("Content-Length");

        if (contentLength.empty())
        {
            Http::RespondWithError::ContentLengthRequired(res);
            return false;
        }

        size_t result{ 0 };

        if (!Http::ParseValue::TryParseIntegral<size_t>(contentLength, &result) ||
            result == 0)
        {
            Http::RespondWithError::BadContentLength(res);
            return false;
        }

        if (result > max)
        {
            Http::RespondWithError::ContentPayloadTooLarge(res);
            return false;
        }

        *outContentLength = result;
        return true;
    }
}