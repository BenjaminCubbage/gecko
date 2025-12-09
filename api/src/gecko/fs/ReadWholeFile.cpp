#include "gecko/fs/ReadWholeFile.h"
#include <fstream>

namespace Gecko::API::FS
{
    std::optional<std::string> ReadWholeFile(const std::string& filepath)
    {
        std::ifstream file{ filepath };

        if (!file.good())
            return std::nullopt;

        file.seekg(0, file.end);
        int fileLen = file.tellg();
        file.seekg(0, file.beg);

        if (!file.good() || !fileLen)
            return std::nullopt;

        std::string str(fileLen, '\0');
        file.read(str.data(), fileLen);

        if (!file.good())
            return std::nullopt;

        return std::move(str);
    }
}