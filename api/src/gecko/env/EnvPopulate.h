#pragma once
#include <ostream>
#include "gecko/env/Env.h"

namespace Gecko::API::Env
{
    class EnvPopulate
    {
    public:
        static std::optional<Env> Populate(const std::string& filepath, 
                                           std::ostream& log, 
                                           const std::optional<std::string>& mosquittoPasswordOverride = std::nullopt,
                                           const std::optional<std::string>& mysqlPasswordOverride = std::nullopt);

    private:
        static bool GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues, 
                                const std::string& key,
                                std::string* outValue,
                                std::ostream& log);
                                
        static bool GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues, 
                                const std::string& key,
                                int* outValue,
                                std::ostream& log);

        static bool GetStringFromFilepath(const std::string& path, 
                                          std::string* outValue, 
                                          std::ostream& log);
    };
}