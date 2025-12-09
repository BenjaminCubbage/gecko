#pragma once
#include <string>
#include <type_traits>
#include <utility>
#include "httplib.h"
#include "json/json.h"
#include "gecko/controllers/respond/RespondWithError.h"

namespace Gecko::API::Controllers::Rules
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
                Respond::RespondWithError::MissingField(res, m_memberName);
                return false;
            }

            Json::ValueType expectedValueType;

            if constexpr (std::is_same<T, std::string>::value)
                expectedValueType = Json::ValueType::stringValue;
            else if constexpr (std::is_same<T, int>::value)
                expectedValueType = Json::ValueType::intValue;

            if (body[m_memberName].type() != expectedValueType)
            {
                Respond::RespondWithError::WrongFieldType(res, m_memberName);
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