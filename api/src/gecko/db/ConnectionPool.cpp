#include "gecko/db/ConnectionPool.h"

namespace Gecko::API::DB
{
    bool ConnectionPool::Start()
    {
        return true;
    }

    ConnectionPool::SessionGuard ConnectionPool::Acquire()
    {
        Session *session{ nullptr };
        std::unique_lock lk{ m_sessionsMutex };

        m_sessionsSignal.wait(lk, [this, &session] {
            for (auto& s : m_sessions)
                if (!s.m_acquired)
                {
                    session      = &s;
                    s.m_acquired = true;
                    return true;
                }

            return false;
        });

        if (!session->m_instance)
            session->m_instance.emplace(OpenSessionInstance());

        return SessionGuard(&*session->m_instance, SessionReleaser{ this, session });
    }

    void ConnectionPool::Release(ConnectionPool::Session& session) noexcept
    {
        {
            std::unique_lock lk{ m_sessionsMutex };
            session.m_acquired      = false;
            session.m_epochLastUsed = EpochNow();
        }

        m_sessionsSignal.notify_one();
    }

    mysqlx::Session ConnectionPool::OpenSessionInstance()
    {
        return mysqlx::Session
        {
            mysqlx::SessionOption::HOST,     m_host,
            mysqlx::SessionOption::PORT,     m_port,
            mysqlx::SessionOption::USER,     m_user,
            mysqlx::SessionOption::PWD,      m_pwd,
            mysqlx::SessionOption::DB,       m_db,
            mysqlx::SessionOption::SSL_MODE, mysqlx::SSLMode::REQUIRED
        };
    }
}