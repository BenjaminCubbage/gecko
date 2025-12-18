#include "gecko/middleware/HasHeader.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    bool HasHeader::operator()(const httplib::Request& req,
                                     httplib::Response& res,
                                     std::string *outHeaderValue)
    {
        *outHeaderValue = req.get_header_value(m_headerName);

        if (!outHeaderValue->size())
        {
            Gecko::API::Http::RespondWithError::MissingHeader(res, m_headerName);
            return false;
        }

        return true;
    }
}