#pragma once
#include <optional>
#include <string>

namespace Gecko::API::Http
{
    class ParseHeader
    {
    public:
        static std::optional<std::string> GetCookieValue(const std::string& header, const std::string& cookieName);
    };
}