#include "gecko/http/MultipartFormData.h"

namespace Gecko::API::Http
{
    MultipartFormData MultipartFormData::FromContentReader(const httplib::ContentReader& reader,
                                                           const std::vector<FieldFinder>& finders)
    {
        std::vector<Field<std::string>>          stringFields;
        std::vector<Field<std::vector<uint8_t>>> vectorFields;

        stringFields.reserve(finders.size());
        vectorFields.reserve(finders.size());

        const FieldFinder* activeFinder{ nullptr };

        reader([&](const httplib::FormData& item) {
            activeFinder = nullptr;

            for (const auto& finder : finders)
            {
                if (finder.fieldName != item.name)
                    continue;

                activeFinder = &finder;

                if (finder.fieldType == FieldFinder::FieldType::String)
                {
                    stringFields.push_back({ finder.fieldName, {} });
                    stringFields.back().content.reserve(finder.guessLength);
                }
                else
                {
                    vectorFields.push_back({ finder.fieldName, {} });
                    vectorFields.back().content.reserve(finder.guessLength);
                }
            }

            return true;
        }, [&](const char* data, size_t length) {
            if (!activeFinder)
                return true;

            if (activeFinder->fieldType == FieldFinder::FieldType::String)
            {
                stringFields.back().content.append(data, length);
            }
            else
            {
                auto& content = vectorFields.back().content;
                content.resize(content.size() + length);
                std::memcpy(content.data() + content.size() - length, data, length);
            }

            return true;
        });

        return MultipartFormData{ std::move(stringFields),
                                  std::move(vectorFields) };
    }

    bool MultipartFormData::find(const std::string& fieldName, std::string** outField)
    {
        *outField = nullptr;

        for (auto& field : m_stringFields)
            if (field.name == fieldName)
                *outField = &field.content;

        return *outField;
    }

    bool MultipartFormData::find(const std::string& fieldName, std::vector<uint8_t>** outField)
    {
        *outField = nullptr;

        for (auto& field : m_vectorFields)
            if (field.name == fieldName)
                *outField = &field.content;

        return *outField;
    }
}