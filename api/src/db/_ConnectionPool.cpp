#include "gecko/db/_ConnectionPool.h"

namespace Gecko::API::DB
{
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