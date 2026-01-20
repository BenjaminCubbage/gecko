#pragma once
#include <string>
#include <type_traits>
#include <utility>
#include "httplib.h"
#include "json/json.h"
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    template<typename T> requires
        std::is_same<T, int>::value ||
        std::is_same<T, std::string>::value
    class HasJSONValueMember
    {
    public:
        HasJSONValueMember(std::string memberName)
            : m_memberName(std::move(memberName)) {}

        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        const Json::Value& body,
                        T* outValue)
        {
            if (!body.isMember(m_memberName))
            {
                Http::RespondWithError::MissingField(res, m_memberName);
                return false;
            }

            Json::ValueType expectedValueType;

            if constexpr (std::is_same<T, std::string>::value)
                expectedValueType = Json::ValueType::stringValue;
            else if constexpr (std::is_same<T, int>::value)
                expectedValueType = Json::ValueType::intValue;

            if (body[m_memberName].type() != expectedValueType)
            {
                Http::RespondWithError::WrongFieldType(res, m_memberName);
                return false;
            }

            if constexpr (std::is_same<T, std::string>::value)
                *outValue = body[m_memberName].asString();
            else if constexpr (std::is_same<T, int>::value)
                *outValue = body[m_memberName].asInt();

            return true;
        }

        const std::string m_memberName;
    };
}