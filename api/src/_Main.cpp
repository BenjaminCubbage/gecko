#include <string>
#include "httplib.h"

#include "_Main.h"
#include "gecko/env/Env.h"
#include "gecko/env/EnvReadFile.h"
#include "gecko/thread/ThreadPool.h"
#include "gecko/controllers/AuthController.h"
#include "gecko/controllers/UsersController.h"
#include "gecko/db/_ConnectionPool.h"
#include "gecko/db/Users.h"
#include "gecko/services/UsersService.h"

#define _STR(x) #x
#define STR(x) _STR(x)

#define MAIN_EXPECT(cond, msg) if (!(cond)) { std::cout << msg << std::endl; return 1; }
#define ENV_EXPECT(env, var) MAIN_EXPECT(env->ContainsKey(var), "[api]: .env file did not contain a " STR(var))
#define USAGE "[api]: usage: api <path/to/.env.local> [<path/to/mosquittorootclientpassword.txt> <path/to/mysqlrootclientpassword.txt>]"

int main(int argc, char* argv[])
{
    using namespace Gecko::API;

    MAIN_EXPECT(argc == 2 || argc == 4, USAGE)
    const auto env = Env::LoadFromFilepath(argv[1]);
    MAIN_EXPECT(env, "[api]: Failed to load .env file at " << argv[1] << ".");

    ENV_EXPECT(env, "GECKO_API_TLS_CERT_PATH");
    ENV_EXPECT(env, "GECKO_API_TLS_CERT_PATH");

    ENV_EXPECT(env, "GECKO_API_TLS_PKEY_PATH");
    ENV_EXPECT(env, "GECKO_API_JWT_PKEY_PATH");
    ENV_EXPECT(env, "GECKO_API_JWT_PUBKEY_PATH");

    ENV_EXPECT(env, "GECKO_API_OAUTH_CLIENTID_PATH");
    ENV_EXPECT(env, "GECKO_API_OAUTH_CLIENTSECRET_PATH");
    ENV_EXPECT(env, "GECKO_API_PORT");
    ENV_EXPECT(env, "MOSQUITTO_PORT");
    ENV_EXPECT(env, "MYSQL_XAPI_PORT"); 

    std::string mosquittoPasswordPath{};
    std::string mysqlPasswordPath{};

    if (argc == 4)
    {
        mosquittoPasswordPath = argv[2];
        mysqlPasswordPath     = argv[3];
    }
    else
    {
        MAIN_EXPECT(env->ContainsKey("GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH"), USAGE);
        MAIN_EXPECT(env->ContainsKey("GECKO_API_MYSQL_ROOT_PASSWORD_PATH"),     USAGE);
        MAIN_EXPECT((*env)["GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH"].size(),    USAGE);
        MAIN_EXPECT((*env)["GECKO_API_MYSQL_ROOT_PASSWORD_PATH"].size(),        USAGE);
        mosquittoPasswordPath = (*env)["GECKO_API_MOSQUITTO_ROOT_PASSWORD_PATH"];
        mysqlPasswordPath     = (*env)["GECKO_API_MYSQL_ROOT_PASSWORD_PATH"];
    }

    const auto mosquittoPassword = EnvReadFile::ReadFile(mosquittoPasswordPath);
    const auto mysqlPassword     = EnvReadFile::ReadFile(mysqlPasswordPath);

    const auto oauthClientID     = EnvReadFile::ReadFile((*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    const auto oauthClientSecret = EnvReadFile::ReadFile((*env)["GECKO_API_OAUTH_CLIENTSECRET_PATH"]);

    const auto jwtPKey   = EnvReadFile::ReadFile((*env)["GECKO_API_JWT_PKEY_PATH"]);
    const auto jwtPubKey = EnvReadFile::ReadFile((*env)["GECKO_API_JWT_PUBKEY_PATH"]);
    
    MAIN_EXPECT(mosquittoPassword, "[api]: Could not load Mosquitto client password from " << mosquittoPasswordPath);
    MAIN_EXPECT(mysqlPassword,     "[api]: Could not load MySQL client pecret path from "  << mysqlPasswordPath);
    
    MAIN_EXPECT(oauthClientID,     "[api]: Could not load OAuth Client ID path from "     << (*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    MAIN_EXPECT(oauthClientSecret, "[api]: Could not load OAuth Client Secret path from " << (*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    
    MAIN_EXPECT(jwtPKey,   "[api]: Failed to load private key at " << (*env)["GECKO_API_PKEY_PATH"]);
    MAIN_EXPECT(jwtPubKey, "[api]: Failed to load public key at "  << (*env)["GECKO_API_PUBKEY_PATH"]);

    httplib::SSLServer httpServer
    (
        (*env)["GECKO_API_TLS_CERT_PATH"].c_str(),
        (*env)["GECKO_API_TLS_PKEY_PATH"].c_str()
    );

    MAIN_EXPECT(httpServer.is_valid(), "[api]: HTTPS Server was DOA. "
                                       "Certs/paths may be invalid or the process may be underpriviledged.\n"
                                    << "[api]:     Cert path: " << (*env)["GECKO_API_TLS_CERT_PATH"] << "\n"
                                    << "[api]:     PKey path: " << (*env)["GECKO_API_TLS_PKEY_PATH"]);

    std::shared_ptr<DB::ConnectionPool> dbConnections 
        = std::make_shared<DB::ConnectionPool>
        (
            "127.0.0.1",
            std::stoi((*env)["MYSQL_XAPI_PORT"]),
            "root",
            *mysqlPassword,
            "Gecko"
        );

    Services::UsersService usersService{ DB::Users{ dbConnections } };
    Controllers::AuthController authController{ usersService, *oauthClientID, *oauthClientSecret, *jwtPKey, *jwtPubKey };
    Controllers::UsersController usersController{ usersService, *jwtPubKey };

    authController.Attach(httpServer);
    usersController.Attach(httpServer);

    std::cout << "[api]: Listening on 0.0.0.0:" << (*env)["GECKO_API_PORT"] << std::endl;
    MAIN_EXPECT
    (
        httpServer.listen("0.0.0.0", std::stoi((*env)["GECKO_API_PORT"])),
        "[api]: Something went wrong while trying to bind and listen on port " << (*env)["API_PORT"] << "."
    );
}