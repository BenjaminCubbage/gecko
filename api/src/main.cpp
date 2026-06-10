#include <iostream>
#include <string>
#include "httplib.h"
#include "gecko/logging/ConsoleLogger.h"
#include "gecko/logging/Logger.h"
#include "gecko/env/Env.h"
#include "gecko/env/EnvPopulate.h"
#include "gecko/server/Server.h"

using namespace Gecko::API;

int main(int argc, char* argv[])
{
    using Logging::Logger;
    using Logging::ConsoleLogger;

    Logger::InitInfo<ConsoleLogger>("[INF] ");
    Logger::InitWarn<ConsoleLogger>("[WRN] ");
    Logger::InitError<ConsoleLogger>("[ERR] ");
    Logger::InitDebug<ConsoleLogger>("[DBG] ");

    if (argc != 2 && argc != 4)
    {
        std::cout << "[main]: Usage: api <path/to/.env.local>\n"
                  << "               api <path/to/.env.local> <path/to/mosquittorootclientpassword.txt> <path/to/mysqlrootclientpassword.txt>"
                  << std::endl;
        return 1;
    }

    const auto env = argc == 2
        ? Env::EnvPopulate::Populate(argv[1])
        : Env::EnvPopulate::Populate(argv[1], argv[2], argv[3]);

    if (!env)
        return 1;
    
    return !Server::Server{ std::move(*env) }.Start();
}