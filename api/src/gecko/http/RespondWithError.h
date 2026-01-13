#pragma once
#include <string>
#include "httplib.h"

namespace Gecko::API::Http
{
    class RespondWithError
    {
    public:
        static inline void ContentLengthRequired(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_content_length", "length_required");
            response.status = httplib::StatusCode::LengthRequired_411;
        }

        static inline void BadContentLength(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_content_length", "not_valid");
            response.status = httplib::StatusCode::LengthRequired_411;
        }

        static inline void ContentPayloadTooLarge(httplib::Response& response)
        {
            response.body   = BuildResponseBody("content_too_large", "too_large");
            response.status = httplib::StatusCode::PayloadTooLarge_413;
        }

        static inline void CouldNotFulfill(httplib::Response& response)
        {
            response.body   = BuildResponseBody("could_not_fulfill", "internal_error");
            response.status = httplib::StatusCode::InternalServerError_500;
        }

        static inline void MalformedPath(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_request", "malformed_path");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void UsernameTaken(httplib::Response& response)
        {
            response.body   = BuildResponseBody("conflict", "username_taken");
            response.status = httplib::StatusCode::Conflict_409;
        }

        static inline void UsernameTooLong(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_request", "username_too_long");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void UsernameTooShort(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_request", "username_too_short");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void UsernameContainsInvalidCharacters(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_request", "username_contains_invalid_characters");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void MissingField(httplib::Response& response, const std::string& fieldName)
        {
            response.body   = BuildResponseBodyWithField("bad_request", "missing_field", fieldName);
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void WrongFieldType(httplib::Response& response, const std::string& fieldName)
        {
            response.body   = BuildResponseBodyWithField("bad_request", "wrong_field_type", fieldName);
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void BadField(httplib::Response& response, const std::string& fieldName)
        {
            response.body   = BuildResponseBodyWithField("bad_request", "bad_field", fieldName);
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void FieldTooLong(httplib::Response& response, const std::string& field)
        {
            response.body   = BuildResponseBodyWithField("bad_request", "field_too_long", field);
            response.status = httplib::StatusCode::PayloadTooLarge_413;
        }

        static inline void MissingHeader(httplib::Response& response, const std::string& headerName)
        {
            response.body   = BuildResponseBodyWithHeader("bad_request", "missing_header", headerName);
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void BadHeader(httplib::Response& response, const std::string& headerName)
        {
            response.body   = BuildResponseBodyWithHeader("bad_request", "missing_header", headerName);
            response.status = httplib::StatusCode::BadRequest_400;
        }
        
        static inline void SelfFriendNotAllowed(httplib::Response& response)
        {
            response.body = BuildResponseBody("bad_request", "self_friend_not_allowed");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void UserNotFound(httplib::Response& response)
        {
            response.body   = BuildResponseBody("not_found", "user_not_found");
            response.status = httplib::StatusCode::NotFound_404;
        }

        static inline void DeviceNotFound(httplib::Response& response)
        {
            response.body   = BuildResponseBody("not_found", "device_not_found");
            response.status = httplib::StatusCode::NotFound_404;
        }

        static inline void FriendRequestNotFound(httplib::Response& response)
        {
            response.body   = BuildResponseBody("not_found", "friend_request_not_found");
            response.status = httplib::StatusCode::NotFound_404;
        }

        static inline void XSRFMissing(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_xsrf", "missing");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void XSRFInvalid(httplib::Response& response)
        {
            response.body   = BuildResponseBody("bad_xsrf", "invalid");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void AuthMissing(httplib::Response& response)
        {
            response.body   = BuildResponseBody("auth_required", "missing");
            response.status = httplib::StatusCode::Unauthorized_401;
        }

        static inline void AuthInvalid(httplib::Response& response)
        {
            response.body   = BuildResponseBody("auth_required", "invalid");
            response.status = httplib::StatusCode::Unauthorized_401;
        }

        static inline void OAuthCancelled(httplib::Response& response)
        {
            response.body   = BuildResponseBody("oauth_failed", "cancelled");
            response.status = httplib::StatusCode::BadRequest_400;
        }

        static inline void OAuthInternalError(httplib::Response& response)
        {
            response.body   = BuildResponseBody("oauth_failed", "internal_error");
            response.status = httplib::StatusCode::InternalServerError_500;
        }

        static inline void Forbidden(httplib::Response& response)
        {
            response.body   = BuildResponseBody("forbidden", "insufficient_permissions");
            response.status = httplib::StatusCode::Forbidden_403;
        }

        static inline void Unprocessable(httplib::Response& response)
        {
            response.body   = BuildResponseBody("unprocessable", "malformed_request");
            response.status = httplib::StatusCode::UnprocessableContent_422;
        }

        static inline std::string BuildResponseBody(const std::string& code, const std::string& reason)
        {
            return R"({"error":{"code":")" + code
                 + R"(","reason":")" + reason
                 + R"("}})";
        }

        static inline std::string BuildResponseBodyWithField(const std::string& code, const std::string& reason, const std::string& field)
        {
            return R"({"error":{"code":")" + code
                 + R"(","reason":")" + reason
                 + R"(","field":")" + field
                 + R"("}})";
        }

        static inline std::string BuildResponseBodyWithHeader(const std::string& code, const std::string& reason, const std::string& field)
        {
            return R"({"error":{"code":")" + code
                 + R"(","reason":")" + reason
                 + R"(","header":")" + field
                 + R"("}})";
        }
    };
}