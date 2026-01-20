#include "gecko/env/EnvPopulate.h"
#include "gecko/env/EnvParse.h"
#include "gecko/fs/ReadWholeFile.h"
#include "gecko/logging/Logger.h"

namespace Gecko::API::Env
{
    using Logging::Logger;

    std::optional<Env> EnvPopulate::Populate(const std::string& filepath,
                                             const std::optional<std::string>& mosquittoPasswordPathOverride,
                                             const std::optional<std::string>& mysqlPasswordPathOverride)
    {
        Env env;

        const auto fileContents = FS::ReadWholeFile(filepath);

        if (!fileContents)
        {
            Logger::Error() << "[EnvPopulate.Populate]: Environment file couldn't be opened: " + filepath;
            return std::nullopt;
        }

        const auto keyValues = EnvParse::Parse(*fileContents);

        if (keyValues.empty())
        {
            Logger::Error() << "[EnvPopulate.Populate]: Environment file could not be parsed: " + filepath;
            return std::nullopt;
        }

        if (!GetKeyValue(keyValues, "GECKO_API_TLS_CERT_PATH",           &env.geckoAPITLSCertPath          )) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_TLS_PKEY_PATH",           &env.geckoAPITLSPkeyPath          )) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_JWT_PKEY_PATH",           &env.geckoAPIJWTPkeyPath          )) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_JWT_PUBKEY_PATH",         &env.geckoAPIJWTPubkeyPath        )) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_OAUTH_CLIENTID_PATH",     &env.geckoAPIOAuthClientIDPath    )) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_OAUTH_CLIENTSECRET_PATH", &env.geckoAPIOAuthClientSecretPath)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_PORT",                    &env.geckoAPIPort                 )) return std::nullopt;
        if (!GetKeyValue(keyValues, "MOSQUITTO_PORT",                    &env.mosquittoPort                )) return std::nullopt;
        if (!GetKeyValue(keyValues, "MYSQL_XAPI_PORT",                   &env.mysqlXAPIPort                )) return std::nullopt;
        if (!GetKeyValue(keyValues, "MOSQUITTO_CERT_PATH",               &env.mosquittoCertPath            )) return std::nullopt;

        if (!mosquittoPasswordPathOverride)
        {
            if (!GetKeyValue(keyValues, "GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH", &env.geckoAPIMosquittoRootPasswordPath))
                return std::nullopt;
        }
        else
            env.geckoAPIMosquittoRootPasswordPath = *mosquittoPasswordPathOverride;

        if (!mysqlPasswordPathOverride)
        {
            if (!GetKeyValue(keyValues, "GECKO_API_MYSQL_ROOT_PASSWORD_PATH", &env.geckoAPIMySQLRootPasswordPath))
                return std::nullopt;
        }
        else
            env.geckoAPIMySQLRootPasswordPath = *mysqlPasswordPathOverride;

        if (!GetStringFromFilepath(env.mosquittoCertPath,                 &env.mosquittoCert))     return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIMosquittoRootPasswordPath, &env.mosquittoPassword)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIMySQLRootPasswordPath,     &env.mysqlPassword))     return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIOAuthClientIDPath,         &env.oauthClientID))     return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIOAuthClientSecretPath,     &env.oauthClientSecret)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIJWTPkeyPath,               &env.jwtPrivateKey))     return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIJWTPubkeyPath,             &env.jwtPublicKey))      return std::nullopt;

        return env;
    }

    bool EnvPopulate::GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues,
                                  const std::string& key,
                                  std::string* outValue)
    {
        const auto it = keyValues.find(key);
        if (it == keyValues.end())
        {
            Logger::Error() << "[EnvPopulate.GetKeyValue]: Environment key not found: " + key;
            return false;
        }

        *outValue = it->second;
        return true;
    }

    bool EnvPopulate::GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues,
                                  const std::string& key,
                                  int *outValue)
    {
        std::string str;

        if (!GetKeyValue(keyValues, key, &str))
            return false;

        try
        {
            *outValue = std::stoi(str);
            return true;
        }
        catch (const std::invalid_argument&)
        {
            Logger::Error() << "[EnvPopulate.GetKeyValue]: Environment key not an integer: " + key;
            return false;
        }
        catch (const std::out_of_range&)
        {
            Logger::Error() << "[EnvPopulate.GetKeyValue]: Environment key integral was out of range: " + key;
            return false;
        }
    }

    bool EnvPopulate::GetStringFromFilepath(const std::string& path,
                                            std::string* outValue)
    {
        const auto contents = FS::ReadWholeFile(path);

        if (!contents)
        {
            Logger::Error() << "[EnvPopulate.GetStringFromFilepath]: Could not load required file: " + path;
            return false;
        }

        *outValue = std::move(*contents);
        return true;
    }
}