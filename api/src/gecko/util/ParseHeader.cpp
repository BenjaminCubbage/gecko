#include "gecko/util/ParseHeader.h"
#include <ranges>
#include <string_view>

namespace Gecko::API::Util
{
    std::optional<std::string> ParseHeader::GetCookieValue(const std::string& header, const std::string& cookieName)
    {
        using namespace std::views;

        static constexpr const std::string delim{ ";" };

        auto cookie = 
            split(header, delim)
          | transform([](auto&& range) {
                auto sv = std::string_view(range.begin(), range.end());
                sv.remove_prefix(std::min(sv.find_first_not_of(' '), sv.size()));
                return sv;
            })
          | filter([&cookieName](auto&& sv) {
                return sv.starts_with(cookieName) && sv.find('=', 1) != std::string::npos;
            })
          | transform([](auto&& sv) {
                auto cp{ sv };
                cp.remove_prefix(std::min(sv.find_first_of('=') + 1, sv.size()));
                return cp;
            });

        if (cookie.begin() == cookie.end())
            return std::nullopt;

        return std::string(*cookie.begin());
    }
}