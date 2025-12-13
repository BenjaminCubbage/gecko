#pragma once
#include "httplib.h"
#include "gecko/http/MultipartFormData.h"

namespace Gecko::API::Middleware
{
    class HasAnyMultipartFormData
    {
    public:
        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        const httplib::ContentReader& reader,
                        std::vector<Http::MultipartFormData::FieldFinder>& finders,
                        Http::MultipartFormData *outMFD);
    };
}