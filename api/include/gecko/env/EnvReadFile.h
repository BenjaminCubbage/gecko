#pragma once
#include <string>
#include <optional>

namespace Gecko::API
{
    class EnvReadFile
    {
    public:
        static std::optional<std::string> ReadFile(const std::string& filepath);
    };
}