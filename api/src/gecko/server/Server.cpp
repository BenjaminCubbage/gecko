#include "gecko/server/Server.h"
#include "gecko/controllers/AuthController.h"
#include "gecko/controllers/SharedImagesController.h"
#include "gecko/controllers/UsersController.h"
#include "gecko/db/ConnectionPool.h"
#include "gecko/db/UsersTable.h"
#include "gecko/services/UsersService.h"

namespace Gecko::API::Server
{
    bool Server::Start(const Env::Env& env, std::ostream& log)
    {
        httplib::SSLServer httpServer
        (
            env.geckoAPITLSCertPath.c_str(),
            env.geckoAPITLSPkeyPath.c_str()
        );
        
        if (!httpServer.is_valid())
        {
            log << "[api]: HTTPS Server was DOA. "
                   "Certs/paths may be invalid or the process may be underpriviledged.\n"
                << "[api]:     Cert path: " << env.geckoAPITLSCertPath << "\n"
                << "[api]:     PKey path: " << env.geckoAPITLSPkeyPath;
            return false;
        }

        auto connectionPool = std::make_shared<DB::ConnectionPool>
        (
            "127.0.0.1",
            env.mysqlXAPIPort,
            "root",
            env.mysqlPassword,
            "Gecko"
        );

        DB::UsersTable dbUsers{ connectionPool };

        Services::UsersService usersService{ dbUsers };
        Services::SharedImagesService sharedImagesService{ dbUsers };

        Controllers::AuthController authController
        { 
            usersService, 
            env.oauthClientID, 
            env.oauthClientSecret, 
            env.jwtPrivateKey,
            env.jwtPublicKey
        };

        Controllers::UsersController usersController{ usersService, env.jwtPublicKey };
        Controllers::SharedImagesController sharedImagesController{ sharedImagesService, env.jwtPublicKey };
        
        authController.Attach(httpServer);
        usersController.Attach(httpServer);
        sharedImagesController.Attach(httpServer);

        log << "[api]: Listening on 0.0.0.0:" << env.geckoAPIPort << std::endl;
        if (!httpServer.listen("0.0.0.0", env.geckoAPIPort))
        {
            log << "[api]: Error: Couldn't bind to port " << env.geckoAPIPort;
            return false;
        }

        return true;
    }
}