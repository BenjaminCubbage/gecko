#pragma once
#include <string>
#include <unordered_map>

namespace Gecko::API::Env
{
    class EnvParse
    {
    public:
        static std::unordered_map<std::string, std::string> Parse(const std::string &str);
    };
}