#include "gecko/server/Tables.h"

namespace Gecko::API::Server
{
    bool Tables::Start()
    {
        if (!m_dbConnectionPool.Start())
        {
            *m_log << "[Tables]: Couldn't connect to the MySQL XAPI server via\n";
            *m_log << "[Tables]: Note: On port " << m_env->mysqlXAPIPort << std::endl;
            return false;
        }

        *m_log << "[Tables]: Successfully connected to the MySQL server\n";
        return true;
    }
}