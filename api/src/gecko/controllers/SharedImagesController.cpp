#include <iostream>
#include "gecko/controllers/SharedImagesController.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/middleware/HasAnyMultipartFormData.h"
#include "gecko/middleware/HasMultipartFormDataField.h"
#include "gecko/middleware/HasContentLength.h"
#include "gecko/http/MultipartFormData.h"

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     SharedImagesController::s_jsonReader{};
    thread_local Json::FastWriter SharedImagesController::s_jsonWriter{};

    void SharedImagesController::Attach(httplib::Server& server)
    {
        server.Post("/api/users/:id/shared-images", [this] (const httplib::Request& req,
                                                            httplib::Response& res,
                                                            const httplib::ContentReader& contentReader) {
            Handle_POST_SharedImages(req, res, contentReader);
        });
    }

    void SharedImagesController::Handle_POST_SharedImages(const httplib::Request& req,
                                                          httplib::Response& res,
                                                          const httplib::ContentReader& contentReader)
    {
        using FieldFinder = Http::MultipartFormData::FieldFinder;

        thread_local std::vector<FieldFinder> mfdFinders{
            { "content",  FieldFinder::FieldType::Vector, 1000 },
            { "metadata", FieldFinder::FieldType::String, 32 }
        };

        size_t contentLength{};
        if (!Middleware::HasContentLength{}(req, res, &contentLength))
            return;

        // Estimate at least 100 bytes for everything that isn't
        // the blob.
        mfdFinders[0].guessLength = std::max(contentLength, std::size_t{ 100 }) - 100;

        Http::MultipartFormData formData;
        std::string *fdMetadata;
        std::vector<uint8_t> *fdContent;

        if (!Middleware::HasAnyMultipartFormData{}(req, res, contentReader, mfdFinders, &formData) ||
            !Middleware::HasMultipartFormDataField<std::string>{ "metadata" }(req, res, formData, &fdMetadata) ||
            !Middleware::HasMultipartFormDataField<std::vector<uint8_t>>{ "content" }(req, res, formData, &fdContent))
            return;

        std::cout << *fdMetadata << std::endl;
        std::cout << "guess:  " << mfdFinders[0].guessLength << "\n";
        std::cout << "actual: " << fdContent->size() << std::endl;
    }
}