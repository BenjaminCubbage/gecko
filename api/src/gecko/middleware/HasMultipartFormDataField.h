#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include "httplib.h"
#include "gecko/http/MultipartFormData.h"

namespace Gecko::API::Middleware
{
    template<typename T> requires
        std::is_same_v<T, std::string> ||
        std::is_same_v<T, std::vector<uint8_t>>
    class HasMultipartFormDataField
    {
      public:
        HasMultipartFormDataField(std::string fieldName)
            : m_fieldName(std::move(fieldName)) {}

        bool operator()(const httplib::Request& req,
                        httplib::Response& res,
                        Http::MultipartFormData& formData,
                        T** outResult);

      private:
        std::string m_fieldName;
    };
}