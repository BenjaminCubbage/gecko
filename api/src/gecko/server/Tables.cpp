#include "gecko/server/Tables.h"
#include <string>
#include <utility>
#include "gecko/logging/Logger.h"

namespace Gecko::API::Server
{
    bool Tables::Start()
    {
        using Logging::Logger;

        if (auto r{ m_dbConnectionPool.Start() }; r != DB::ConnectionPool::Result::OK)
        {
            Logger::Error() << "[Tables.Start]: Couldn't connect to the MySQL XAPI server";
            Logger::Error() << "[Tables.Start]: ~ On port:    " + std::to_string(m_env->mysqlXAPIPort);
            Logger::Error() << "[Tables.Start]: ~ Error code: " + std::to_string(std::to_underlying(r));
            return false;
        }

        Logger::Info() << "[Tables.Start]: Successfully initialized MySQL connection pool";
        return true;
    }
}