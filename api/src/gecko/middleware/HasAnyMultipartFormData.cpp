#include "gecko/middleware/HasAnyMultipartFormData.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    bool HasAnyMultipartFormData::operator()(const httplib::Request& req,
                                             httplib::Response& res,
                                             const httplib::ContentReader& reader,
                                             std::vector<Http::MultipartFormData::FieldFinder>& finders,
                                             Http::MultipartFormData *outMFD)
    {
        if (!req.is_multipart_form_data())
        {
            Http::RespondWithError::Unprocessable(res);
            return false;
        }

        *outMFD = Http::MultipartFormData::FromContentReader(reader, finders);
        return true;
    }
}