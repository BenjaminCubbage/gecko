#pragma once
#include <optional>
#include <string>

namespace Gecko::API::Env
{
    struct Env
    {
        // TLS / HTTPS configuration
        std::string geckoAPITLSCertPath;
        std::string geckoAPITLSPkeyPath;

        // HTTP Origins
        std::string geckoAPIBackendOrigin;
        std::string geckoAPIFrontendOrigin;

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

        // MQTT TLS
        std::string mosquittoCertPath;

        // Resolved secret contents
        std::string mosquittoCert;
        std::string mosquittoPassword;
        std::string mysqlPassword;
        std::string oauthClientID;
        std::string oauthClientSecret;
        std::string jwtPrivateKey;
        std::string jwtPublicKey;
    };
}