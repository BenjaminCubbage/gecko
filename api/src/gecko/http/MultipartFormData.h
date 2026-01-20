#pragma once
#include <string>
#include <vector>
#include "httplib.h"

namespace Gecko::API::Http
{
    class MultipartFormData
    {
    public:
        template<typename TData>
        struct Field
        {
            std::string name;
            TData content;
        };

        struct FieldFinder
        {
            enum class FieldType { String, Vector };

            FieldFinder(std::string fieldName, FieldType fieldType, size_t guessLength)
                : fieldName(std::move(fieldName)), fieldType(fieldType), guessLength(guessLength) {}

            std::string fieldName;
            FieldType fieldType;
            size_t guessLength{ 0 };
        };

        static MultipartFormData FromContentReader(const httplib::ContentReader& reader,
                                                   const std::vector<FieldFinder>& finders);

        MultipartFormData() {}

        bool Find(const std::string& fieldName, std::string** outField);
        bool Find(const std::string& fieldName, std::vector<uint8_t>** outField);

    private:
        MultipartFormData(std::vector<Field<std::string>> stringFields,
                          std::vector<Field<std::vector<uint8_t>>> vectorFields)
            : m_stringFields(std::move(stringFields)), m_vectorFields(std::move(vectorFields)) {}

        std::vector<Field<std::string>>          m_stringFields;
        std::vector<Field<std::vector<uint8_t>>> m_vectorFields;
    };
}