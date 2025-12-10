#include <string>
#include "httplib.h"
#include "gecko/env/Env.h"
#include "gecko/env/EnvPopulate.h"
#include "gecko/server/Server.h"

#define _STR(x) #x
#define STR(x) _STR(x)

#define MAIN_EXPECT(cond, msg) if (!(cond)) { std::cout << msg << std::endl; return 1; }
#define ENV_EXPECT(env, var) MAIN_EXPECT(env->ContainsKey(var), "[api]: .env file did not contain a " STR(var))

using namespace Gecko::API;

int main(int argc, char* argv[])
{
    std::ostream& log = std::cout;

    if (argc != 2 && argc != 4)
        std::cout << "[api]: Usage: api <path/to/.env.local> " 
                  << "              api <path/to/.env.local> <path/to/mosquittorootclientpassword.txt> <path/to/mysqlrootclientpassword.txt>";

    const auto env = argc == 2
        ? Env::EnvPopulate::Populate(argv[1], std::cout)
        : Env::EnvPopulate::Populate(argv[1], std::cout, argv[2], argv[3]);

    if (!env)
        return 1;

    return !Server::Server{}.Start(*env, log);
}