#include "gecko/server/Server.h"
#include <atomic>
#include <chrono>
#include <memory>
#include "httplib.h"

namespace Gecko::API::Server
{
    bool Server::Start()
    {
        using namespace std::chrono_literals;

        m_threadPool.Start();
        m_scheduler.Start();

        if (!m_topics.Start() ||
            !m_tables.Start() ||
            !m_controllers.Start(m_httpServer))
        {
            *m_log << "[Server]: Failed to start server; See previous output" << std::endl;
            return false;
        }
        
        if (!m_httpServer.is_valid())
        {
            *m_log << "[Server]: HTTPS server was DOA. \n";
            *m_log << "[Server]: Certs may be invalid or the process may be underpriviledged\n";
            *m_log << "[Server]:    Cert path: " << m_env.geckoAPITLSCertPath << "\n";
            *m_log << "[Server]:    PKey path: " << m_env.geckoAPITLSPkeyPath << std::endl;
            return false;
        }

        *m_log << "[Server]: Successfully initialized HTTPS server" << std::endl;
        *m_log << "[Server]: Preparing to listen on port " << m_env.geckoAPIPort << std::endl;

        if (!m_httpServer.listen("0.0.0.0", m_env.geckoAPIPort))
        {
            *m_log << "[Server]: Failed to listen or bind SSL server";
            return false;
        }

        return true;
    }
}