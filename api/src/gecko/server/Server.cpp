#include "gecko/server/Server.h"
#include <chrono>
#include <memory>
#include "httplib.h"
#include "gecko/controllers/AuthController.h"
#include "gecko/controllers/DevicesController.h"
#include "gecko/controllers/SharedImagesController.h"
#include "gecko/controllers/UsersController.h"
#include "gecko/db/ConnectionPool.h"
#include "gecko/db/DevicesTable.h"
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/mqtt/MQTTClient.h"
#include "gecko/services/DevicesService.h"
#include "gecko/services/SharedImagesService.h"
#include "gecko/services/UsersService.h"
#include "gecko/topics/DevicesHeartbeatTopic.h"

namespace Gecko::API::Server
{
    bool Server::Start(const Env::Env& env, std::ostream& log)
    {
        auto mqttClient = std::make_shared<MQTT::MQTTClient>(
            "ssl://localhost:" + std::to_string(env.mosquittoPort),
            env.mosquittoCertPath,
            "root",
            env.mosquittoPassword);

        if (!mqttClient->ConnectSync())
        {

            log << "[api]: Couldn't connect to the MQTT server on port " 
                << env.mosquittoPort << ": " << mqttClient->m_connectFailedCode << std::endl;
            return false;
        }

        log << "[api]: Connected to MQTT server on port " << env.mosquittoPort << std::endl;

        auto devicesHeartbeatTopic = std::make_shared<Topics::DevicesHeartbeatTopic>(mqttClient);
        auto connectionPool = std::make_shared<DB::ConnectionPool>(
            "127.0.0.1", env.mysqlXAPIPort, "root", env.mysqlPassword, "Gecko");

        DB::UsersTable        dbUsers       { connectionPool };
        DB::SharedImagesTable dbSharedImages{ connectionPool };
        DB::DevicesTable      dbDevices     { connectionPool };

        Services::UsersService        usersService       { dbUsers };
        Services::SharedImagesService sharedImagesService{ dbSharedImages, dbUsers };
        Services::DevicesService      devicesService     { devicesHeartbeatTopic, dbDevices, dbUsers };

        httplib::SSLServer httpServer(
            env.geckoAPITLSCertPath.c_str(),
            env.geckoAPITLSPkeyPath.c_str());

        httpServer.set_keep_alive_timeout(10);
        httpServer.set_keep_alive_max_count(50);
        httpServer.set_tcp_nodelay(true);

        if (!httpServer.is_valid())
        {
            log << "[api]: HTTPS Server was DOA. "
                   "Certs/paths may be invalid or the process may be underpriviledged.\n"
                << "[api]:     Cert path: " << env.geckoAPITLSCertPath << "\n"
                << "[api]:     PKey path: " << env.geckoAPITLSPkeyPath;
            return false;
        }

        Controllers::AuthController authController{
            usersService,
            env.oauthClientID,
            env.oauthClientSecret,
            env.jwtPrivateKey,
            env.jwtPublicKey };

        Controllers::UsersController        usersController       { usersService,        env.jwtPublicKey };
        Controllers::SharedImagesController sharedImagesController{ sharedImagesService, env.jwtPublicKey };
        Controllers::DevicesController      devicesController     { devicesService,      env.jwtPublicKey };

        authController        .Attach(httpServer);
        usersController       .Attach(httpServer);
        sharedImagesController.Attach(httpServer);
        devicesController     .Attach(httpServer);

        log << "[api]: HTTP server listening on 0.0.0.0:" << env.geckoAPIPort << std::endl;
        if (!httpServer.listen("0.0.0.0", env.geckoAPIPort))
        {
            log << "[api]: Error: Couldn't bind to port " << env.geckoAPIPort;
            return false;
        }

        return true;
    }
}