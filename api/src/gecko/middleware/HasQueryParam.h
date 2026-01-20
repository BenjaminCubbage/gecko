#pragma once
#include <type_traits>
#include "gecko/http/RespondWithError.h"
#include "gecko/http/ParseValue.h"
#include "httplib.h"

namespace Gecko::API::Middleware
{
    template<typename T>
    class HasQueryParam;

    template<typename T> requires
        std::is_same_v<T, std::string>
    class HasQueryParam<T>
    {
    public:
        HasQueryParam(std::string paramName)
            : m_paramName(std::move(paramName)) {}

        bool operator()(const httplib::Request& req, httplib::Response& res, std::string* outResult)
        {
            if (!req.has_param(m_paramName))
            {
                Http::RespondWithError::MissingField(res, m_paramName);
                return false;
            }

            *outResult = req.get_param_value(m_paramName);
            return true;
        }

    private:
        std::string m_paramName;
    };

    template<typename T> requires
        std::is_integral_v<T>
    class HasQueryParam<T>
    {
    public:
        HasQueryParam(std::string paramName, size_t maxLength)
            : m_paramName(std::move(paramName)),
              m_maxLength(maxLength) {}

        bool operator()(const httplib::Request& req, httplib::Response& res, T* outResult)
        {
            if (!req.has_param(m_paramName))
            {
                Http::RespondWithError::MissingField(res, m_paramName);
                return false;
            }

            std::string paramValue = req.get_param_value(m_paramName);

            if (!Http::ParseValue::TryParseIntegral<T>(outResult))
            {
                Http::RespondWithError::BadField(res, m_paramName);
                return false;
            }

            return true;
        }

    private:
        std::string m_paramName;
        size_t m_maxLength;
    };
}