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

#define main_expect(cond, msg) if (!(cond)) { std::cout << msg << std::endl; return 1; }
#define env_expect(env, var) main_expect(env->ContainsKey(var), "[api]: .env file did not contain a " STR(var))

int main(int argc, char* argv[])
{
    using namespace Gecko::API;

    main_expect(argc >= 2, "[api]: Supply a .env file.")
    const auto env = Env::LoadFromFilepath(argv[1]);

    main_expect(env, "[api]: Failed to load .env file at " << argv[1] << ".");

    env_expect(env, "GECKO_API_TLS_CERT_PATH");
    env_expect(env, "GECKO_API_TLS_CERT_PATH");

    env_expect(env, "GECKO_API_TLS_PKEY_PATH");
    env_expect(env, "GECKO_API_JWT_PKEY_PATH");
    env_expect(env, "GECKO_API_JWT_PUBKEY_PATH");

    env_expect(env, "GECKO_API_OAUTH_CLIENTID_PATH");
    env_expect(env, "GECKO_API_OAUTH_CLIENTSECRET_PATH");
    env_expect(env, "GECKO_API_PORT");
    env_expect(env, "MOSQUITTO_PORT");
    env_expect(env, "MYSQL_XAPI_PORT"); 

    const auto oauthClientID     = EnvReadFile::ReadFile((*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    const auto oauthClientSecret = EnvReadFile::ReadFile((*env)["GECKO_API_OAUTH_CLIENTSECRET_PATH"]);

    const auto jwtPKey   = EnvReadFile::ReadFile((*env)["GECKO_API_JWT_PKEY_PATH"]);
    const auto jwtPubKey = EnvReadFile::ReadFile((*env)["GECKO_API_JWT_PUBKEY_PATH"]);
    
    main_expect(oauthClientID,     "[api]: Could not load OAuth Client ID path from "     << (*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    main_expect(oauthClientSecret, "[api]: Could not load OAuth Client Secret path from " << (*env)["GECKO_API_OAUTH_CLIENTID_PATH"]);
    
    main_expect(jwtPKey,   "[api]: Failed to load private key at " << (*env)["GECKO_API_PKEY_PATH"]);
    main_expect(jwtPubKey, "[api]: Failed to load public key at "  << (*env)["GECKO_API_PUBKEY_PATH"]);

    httplib::SSLServer httpServer
    (
        (*env)["GECKO_API_TLS_CERT_PATH"].c_str(),
        (*env)["GECKO_API_TLS_PKEY_PATH"].c_str()
    );

    main_expect(httpServer.is_valid(), "[api]: HTTPS Server was DOA. "
                                       "Certs/paths may be invalid or the process may be underpriviledged.\n"
                                    << "[api]:     Cert path: " << (*env)["GECKO_API_TLS_CERT_PATH"] << "\n"
                                    << "[api]:     PKey path: " << (*env)["GECKO_API_TLS_PKEY_PATH"]);

    std::shared_ptr<DB::ConnectionPool> dbConnections 
        = std::make_shared<DB::ConnectionPool>
        (
            "127.0.0.1",
            std::stoi((*env)["MYSQL_XAPI_PORT"]),
            "root",
            "1",
            "Gecko"
        );

    Services::UsersService usersService{ DB::Users{ dbConnections } };
    Controllers::AuthController authController{ usersService, *oauthClientID, *oauthClientSecret, *jwtPKey, *jwtPubKey };
    Controllers::UsersController usersController{ usersService, *jwtPubKey };

    authController.Attach(httpServer);
    usersController.Attach(httpServer);

    std::cout << "[api]: Listening on 0.0.0.0:" << (*env)["GECKO_API_PORT"] << std::endl;
    main_expect
    (
        httpServer.listen("0.0.0.0", std::stoi((*env)["GECKO_API_PORT"])),
        "[api]: Something went wrong while trying to bind and listen on port " << (*env)["API_PORT"] << "."
    );
}