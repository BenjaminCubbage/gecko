#include "gecko/env/EnvPopulate.h"
#include "gecko/env/EnvParse.h"
#include "gecko/fs/ReadWholeFile.h"

namespace Gecko::API::Env
{
    std::optional<Env> EnvPopulate::Populate(const std::string& filepath, 
                                             std::ostream& log,
                                             const std::optional<std::string>& mosquittoPasswordPathOverride,
                                             const std::optional<std::string>& mysqlPasswordPathOverride)
    {
        Env env;

        const auto fileContents = FS::ReadWholeFile(filepath);

        if (!fileContents)
        {
            log << "[api]: Environment file contents couldn't be read: " << filepath << std::endl;
            return std::nullopt;
        }

        const auto keyValues = EnvParse::Parse(*fileContents);

        if (keyValues.empty())
        {
            log << "[api]: Environment file contained no parseable keys: " << filepath << std::endl;
            return std::nullopt;
        }

        if (!GetKeyValue(keyValues, "GECKO_API_TLS_CERT_PATH",           &env.geckoAPITLSCertPath,           log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_TLS_PKEY_PATH",           &env.geckoAPITLSPkeyPath,           log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_JWT_PKEY_PATH",           &env.geckoAPIJWTPkeyPath,           log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_JWT_PUBKEY_PATH",         &env.geckoAPIJWTPubkeyPath,         log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_OAUTH_CLIENTID_PATH",     &env.geckoAPIOAuthClientIDPath,     log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_OAUTH_CLIENTSECRET_PATH", &env.geckoAPIOAuthClientSecretPath, log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "GECKO_API_PORT",                    &env.geckoAPIPort,                  log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "MOSQUITTO_PORT",                    &env.mosquittoPort,                 log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "MYSQL_XAPI_PORT",                   &env.mysqlXAPIPort,                 log)) return std::nullopt;
        if (!GetKeyValue(keyValues, "MOSQUITTO_CERT_PATH",               &env.mosquittoCertPath,             log)) return std::nullopt;

        if (!mosquittoPasswordPathOverride)
        {
            if (!GetKeyValue(keyValues, "GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH", &env.geckoAPIMosquittoRootPasswordPath, log)) 
                return std::nullopt;
        }
        else 
            env.geckoAPIMosquittoRootPasswordPath = *mosquittoPasswordPathOverride;

        if (!mysqlPasswordPathOverride)
        {
            if (!GetKeyValue(keyValues, "GECKO_API_MYSQL_ROOT_PASSWORD_PATH", &env.geckoAPIMySQLRootPasswordPath, log)) 
                return std::nullopt;
        }
        else 
            env.geckoAPIMySQLRootPasswordPath = *mysqlPasswordPathOverride;

        if (!GetStringFromFilepath(env.mosquittoCertPath,                 &env.mosquittoCert,     log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIMosquittoRootPasswordPath, &env.mosquittoPassword, log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIMySQLRootPasswordPath,     &env.mysqlPassword,     log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIOAuthClientIDPath,         &env.oauthClientID,     log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIOAuthClientSecretPath,     &env.oauthClientSecret, log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIJWTPkeyPath,               &env.jwtPrivateKey,     log)) return std::nullopt;
        if (!GetStringFromFilepath(env.geckoAPIJWTPubkeyPath,             &env.jwtPublicKey,      log)) return std::nullopt;

        return env;
    }

    bool EnvPopulate::GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues, 
                                  const std::string& key,
                                  std::string* outValue,
                                  std::ostream& log)
    {
        const auto it = keyValues.find(key);
        if (it == keyValues.end())
        {
            log << "[api]: Environment key " << key << " was not found" << std::endl;
            return false;
        }

        *outValue = it->second;
        return true;
    }

    bool EnvPopulate::GetKeyValue(const std::unordered_map<std::string, std::string>& keyValues, 
                                  const std::string& key,
                                  int *outValue,
                                  std::ostream& log)
    {
        std::string str;

        if (!GetKeyValue(keyValues, key, &str, log))
            return false;

        try
        {
            *outValue = std::stoi(str);
            return true;
        }
        catch (const std::invalid_argument&)
        {
            log << "[api]: Environment key " << key << "was not an integer" << std::endl;
            return false;
        }
        catch (const std::out_of_range&)
        {
            log << "[api]: Environment key " << key << "was out of range" << std::endl;
            return false;
        }
    }
    
    bool EnvPopulate::GetStringFromFilepath(const std::string& path, 
                                            std::string* outValue,
                                            std::ostream& log)
    {
        const auto contents = FS::ReadWholeFile(path);

        if (!contents)
        {
            log << "[api]: Could not load required file at " << path << std::endl;
            return false;
        }

        *outValue = std::move(*contents);
        return true;
    }
}