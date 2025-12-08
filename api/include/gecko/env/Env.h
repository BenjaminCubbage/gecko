#pragma once
#include <unordered_map>
#include <string>
#include <optional>


namespace Gecko::API
{
    class Env
    {
    private:
        Env(){};
        std::unordered_map<std::string, std::string> m_env;

    public:
        static std::optional<Env> LoadFromFilepath(const std::string& filepath);
        static std::optional<Env> LoadFromString(const std::string& str);

        bool ContainsKey(const std::string& key) const
        {
            return m_env.contains(key);
        }

        const std::string& operator[](const std::string& key) const
        {
            return m_env.at(key);
        }
    };
}