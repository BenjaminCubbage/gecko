#pragma once
#include <string>
#include <optional>

namespace Gecko::API::Util
{
    class ParseHeader
    {
    public:
        static std::optional<std::string> GetCookieValue(const std::string& header, const std::string& cookieName);
    };
}