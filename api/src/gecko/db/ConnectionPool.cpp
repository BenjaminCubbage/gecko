#include "gecko/db/ConnectionPool.h"

namespace Gecko::API::DB
{
    bool ConnectionPool::Connect()
    {
        try
        {
            m_sessions.reserve(PoolSize);

            for (size_t i = 0; i < PoolSize; ++i)
                m_sessions.emplace_back
                (
                    mysqlx::Session
                    {
                        mysqlx::SessionOption::HOST,     m_host,
                        mysqlx::SessionOption::PORT,     m_port,
                        mysqlx::SessionOption::USER,     m_user,
                        mysqlx::SessionOption::PWD,      m_pwd,
                        mysqlx::SessionOption::DB,       m_db,
                        mysqlx::SessionOption::SSL_MODE, mysqlx::SSLMode::REQUIRED
                    }
                );

            return true;
        }
        catch (mysqlx::Error&) { }

        return false;
    }

    ConnectionPool::SessionGuard ConnectionPool::Acquire()
    {
        Session *session{ nullptr };
        std::unique_lock lk{ m_sessionsMutex };

        m_sessionsSignal.wait(lk, [this, &session] {
            for (auto& s : m_sessions)
                if (!s.m_inUse)
                {
                    session = &s;
                    s.m_inUse = true;
                    return true;
                }

            return false;
        });

        return SessionGuard{ &session->m_instance, SessionReleaser{ this, session } };
    }

    void ConnectionPool::Release(ConnectionPool::Session& session) noexcept
    {
        {
            std::unique_lock lk{ m_sessionsMutex };
            session.m_inUse = false;
        }

        m_sessionsSignal.notify_one();
    }
}