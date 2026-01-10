#include "gecko/server/Tables.h"

namespace Gecko::API::Server
{
    bool Tables::Start()
    {
        if (auto r{ m_dbConnectionPool.Start() }; r != DB::ConnectionPool::Result::Success)
        {
            *m_log << "[Tables]: Couldn't connect to the MySQL XAPI server\n";
            *m_log << "[Tables]:     On port:    " << m_env->mysqlXAPIPort << '\n';
            *m_log << "[Tables]:     Error code: " << (int)r << std::endl;
            return false;
        }

        *m_log << "[Tables]: Successfully connected to the MySQL server\n";
        return true;
    }
}