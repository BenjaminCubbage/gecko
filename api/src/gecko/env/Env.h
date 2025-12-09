#pragma once
#include <unordered_map>
#include <string>
#include <optional>

namespace Gecko::API::Env
{
    struct Env
    {
        // TLS / HTTPS configuration
        std::string geckoAPITLSCertPath;
        std::string geckoAPITLSPkeyPath;

        // JWT (auth keypair)
        std::string geckoAPIJWTPkeyPath;
        std::string geckoAPIJWTPubkeyPath;

        // OAuth credentials
        std::string geckoAPIOAuthClientIDPath;
        std::string geckoAPIOAuthClientSecretPath;

        // Port configuration
        int geckoAPIPort;
        int mosquittoPort;
        int mysqlXAPIPort;

        // Password paths
        std::string geckoAPIMosquittoRootPasswordPath;
        std::string geckoAPIMySQLRootPasswordPath;

        // Resolved secret contents
        std::string mosquittoPassword;
        std::string mysqlPassword;
        std::string oauthClientID;
        std::string oauthClientSecret;
        std::string jwtPrivateKey;
        std::string jwtPublicKey;
    };
}