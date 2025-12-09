#pragma once
#include <optional>
#include <string>

namespace Gecko::API::FS
{
    std::optional<std::string> ReadWholeFile(const std::string& filepath);
}