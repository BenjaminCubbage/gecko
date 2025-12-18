#include "gecko/middleware/HasMultipartFormDataField.h"
#include <memory>
#include "gecko/http/RespondWithError.h"

namespace Gecko::API::Middleware
{
    template<class T> requires
        std::is_same_v<T, std::string> ||
        std::is_same_v<T, std::vector<uint8_t>>
    bool HasMultipartFormDataField<T>::operator()(const httplib::Request& req,
                                                  httplib::Response& res,
                                                  Http::MultipartFormData& formData,
                                                  T** outResult)
    {
        if (!formData.Find(m_fieldName, outResult))
        {
            Http::RespondWithError::MissingField(res, m_fieldName);
            return false;
        }

        return true;
    }

    template class HasMultipartFormDataField<std::string>;
    template class HasMultipartFormDataField<std::vector<uint8_t>>;
}