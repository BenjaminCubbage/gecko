#include "gecko/middleware/HasContentLength.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    bool HasContentLength::operator()(const httplib::Request& req,
                                      httplib::Response& res,
                                      size_t *outContentLength)
    {
        const auto contentLength = req.get_header_value("Content-Length");

        if (contentLength.empty())
        {
            Http::RespondWithError::BadContentLength(res);
            return false;
        }

        if (contentLength.size() > 7)
        {
            // The main purpose of this class isn't to check the
            // length of the request. This is just a sanity check
            // on the size of content length to prevent overflow
            // before we attempt to parse out its value

            // To truly check that the content length is a reasonable
            // size we should use another middleware component created
            // just for that
            Http::RespondWithError::ContentPayloadTooLarge(res);
            return false;
        }

        size_t result{ 0 };

        for (char c : contentLength)
        {
            if (c > '9' || c < '0')
            {
                Http::RespondWithError::BadContentLength(res);
                return false;
            }

            result = result * 10 + (c - '0');
        }
        
        if (result == 0)
        {
            Http::RespondWithError::BadContentLength(res);
            return false;
        }

        *outContentLength = result;
        return result;
    }
}