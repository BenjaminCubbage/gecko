#include <iostream>
#include "gecko/controllers/SharedImagesController.h"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"
#include "gecko/http/Constants.h"
#include "gecko/middleware/HasAnyMultipartFormData.h"
#include "gecko/middleware/HasContentLength.h"
#include "gecko/middleware/HasHeader.h"
#include "gecko/middleware/HasMultipartFormDataField.h"
#include "gecko/middleware/PathParamEquals.h"
#include "gecko/middleware/UserIsLoggedIn.h"
#include "gecko/http/MultipartFormData.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Controllers
{
    thread_local Json::Reader     SharedImagesController::s_jsonReader{};
    thread_local Json::FastWriter SharedImagesController::s_jsonWriter{};
    
    using ::Gecko::API::Http::Constants::Headers;

    void SharedImagesController::Attach(httplib::Server& server)
    {
        server.Post("/api/users/:id/shared-images", [this] (const httplib::Request& req,
                                                            httplib::Response& res,
                                                            const httplib::ContentReader& contentReader) {
            Handle_POST_SharedImages(req, res, contentReader);
        });
        
        server.Get("/api/users/:id/latest-image", [this] (const httplib::Request& req,
                                                          httplib::Response& res) {
            Handle_GET_LatestImageBlob(req, res);
        });
    }

    void SharedImagesController::Handle_POST_SharedImages(const httplib::Request& req,
                                                          httplib::Response& res,
                                                          const httplib::ContentReader& contentReader)
    {
        using Services::SharedImagesService;
        using FieldFinder = Http::MultipartFormData::FieldFinder;

        thread_local std::vector<FieldFinder> mfdFinders{
            { "content",  FieldFinder::FieldType::Vector, 1000 },
            { "metadata", FieldFinder::FieldType::String, 32 }
        };

        int userID{};
        size_t contentLength{};
        std::string idempotencyKey;
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::HasContentLength{}(req, res, &contentLength) ||
            !Middleware::HasHeader{ Headers::IdempotencyKey }(req, res, &idempotencyKey) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
        {
            return;
        }

        // Avoid copies by estimating the blob's size.
        // Always at least 100 bytes in the payload that aren't the blob.
        mfdFinders[0].guessLength = std::max(contentLength, std::size_t{ 100 }) - 100;

        Http::MultipartFormData formData;
        std::string *fdMetadata;
        std::vector<uint8_t> *fdContent;

        if (!Middleware::HasAnyMultipartFormData{}(req, res, contentReader, mfdFinders, &formData) ||
            !Middleware::HasMultipartFormDataField<std::string>{ "metadata" }(req, res, formData, &fdMetadata) ||
            !Middleware::HasMultipartFormDataField<std::vector<uint8_t>>{ "content" }(req, res, formData, &fdContent))
        {
            return;
        }

        switch (m_sharedImagesService.CreateSharedImage(userID, userID, idempotencyKey, *fdContent))
        {
            case SharedImagesService::Result::OK:
                res.status = httplib::StatusCode::Created_201;
                return;

            case SharedImagesService::Result::IdempotencyKeyReplayed:
                res.status = httplib::StatusCode::OK_200;
                return;

            case SharedImagesService::Result::BadIdempotencyKey:
                Http::RespondWithError::BadHeader(res, Headers::IdempotencyKey);
                return;

            case SharedImagesService::Result::SenderNotFound:
            case SharedImagesService::Result::ReceiverNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
    
    void SharedImagesController::Handle_GET_LatestImageBlob(const httplib::Request& req, 
                                                            httplib::Response& res)
    {
        using Services::SharedImagesService;

        int userID{};
        if (!Middleware::UserIsLoggedIn<int>{ m_pubkey }(req, res, &userID) ||
            !Middleware::PathParamEquals{ "id" }(req, res, std::to_string(userID)))
            return;

        // note(ben): We call std::string template overload to avoid a copy into res.body
        std::vector<uint8_t> bytes;
        switch (m_sharedImagesService.GetLatestReceivedImageBlob(userID, &bytes))
        {
            case SharedImagesService::Result::OK:
                res.set_content(
                    reinterpret_cast<char*>(bytes.data()), 
                    bytes.size(), 
                    "application/octet-stream");
                return;

            case SharedImagesService::Result::ReceiverNotFound:
                Http::RespondWithError::UserNotFound(res);
                return;

            default:
                Http::RespondWithError::CouldNotFulfill(res);
                return;
        }
    }
}