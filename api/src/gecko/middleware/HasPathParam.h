#pragma once
#include <limits>
#include <string>
#include <type_traits>
#include "httplib.h"
#include "gecko/http/ParseValue.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    template<typename T>
    class HasPathParam;

    template<typename T> requires
        std::is_same_v<T, std::string> ||
        std::is_same_v<T, std::string_view>
    class HasPathParam<T>
    {
    public:
        HasPathParam(std::string paramName, size_t maxLength)
            : m_paramName(std::move(paramName)), m_maxLength(maxLength) {}

        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        T* outPathParam)
        {
            const auto value = req.path_params.find(m_paramName);

            if (value == req.path_params.end() || !value->second.size())
            {
                Http::RespondWithError::MalformedPath(res);
                return false;
            }

            if (value->second.size() > m_maxLength)
            {
                Http::RespondWithError::ContentPayloadTooLarge(res);
                return false;
            }

            *outPathParam = value->second;
            return true;
        }

        const std::string m_paramName;
        const size_t m_maxLength;
    };

    template<typename T> requires
        std::is_integral_v<T>
    class HasPathParam<T>
    {
    public:
        HasPathParam(std::string paramName)
            : m_paramName(std::move(paramName)) {}

        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        T* outPathParam)
        {
            const auto value = req.path_params.find(m_paramName);

            if (value == req.path_params.end() || !value->second.size())
            {
                Http::RespondWithError::MalformedPath(res);
                return false;
            }

            if (value->second.size() > std::numeric_limits<T>::digits10 + 1 || 
                !Http::ParseValue::TryParseIntegral(value->second, outPathParam))
            {
                Http::RespondWithError::Unprocessable(res);
                return false;
            }

            return true;
        }

        const std::string m_paramName;
    };
}