#include "gecko/server/Server.h"
#include <atomic>
#include <chrono>
#include <memory>
#include "httplib.h"
#include "gecko/logging/Logger.h"

namespace Gecko::API::Server
{
    bool Server::Start()
    {
        using Logging::Logger;

        if (m_threadPool.Start() != Thread::ThreadPool::Result::OK ||
            m_scheduler.Start() != Thread::Scheduler::Result::OK)
        {
            Logger::Error() << "[Server.Start] Failed to start thread pool or scheduler";
            return false;
        }

        Logger::Info() << "[Server.Start]: Started thread pool and scheduler";

        if (!m_topics.Start() ||
            !m_tables.Start() ||
            !m_controllers.Start(m_httpServer))
        {
            Logger::Error() << "[Server.Start]: Failed to start server; See previous output";
            return false;
        }

        if (!m_httpServer.is_valid())
        {
            Logger::Error() << "[Server.Start]: HTTPS server was DOA";
            Logger::Error() << "[Server.Start]: Certs may be invalid or the process may be underpriviledged";
            Logger::Error() << "[Server.Start]: ~ Cert path: " + m_env.geckoAPITLSCertPath;
            Logger::Error() << "[Server.Start]: ~ Pkey path: " + m_env.geckoAPITLSPkeyPath;
            return false;
        }

        Logger::Info() << "[Server.Start]: Successfully initialized the HTTPS server";
        Logger::Info() << "[Server.Start]: Preparing to listen on port " + std::to_string(m_env.geckoAPIPort);

        if (!m_httpServer.listen("0.0.0.0", m_env.geckoAPIPort))
        {
            Logger::Error() << "[Server.Start]: Failed to listen or bind SSL server";
            return false;
        }

        return true;
    }
}