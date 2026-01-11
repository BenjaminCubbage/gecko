#include "gecko/db/ConnectionPool.h"
#include "gecko/logging/Logger.h"
#include <algorithm>
#include <assert.h>

namespace Gecko::API::DB
{
    using Logging::Logger;

    ConnectionPool::Result ConnectionPool::Start()
    {
        std::unique_lock lk{ m_mutex };

        if (m_state != State::Stopped)
            return Result::StartDenied_NotIdle;

        m_state = State::Starting;

        lk.unlock();
        if (!SessionTimeoutJob_Start())
            return Result::StartFailed_CouldNotStartTimeoutJob;
        lk.lock();

        m_state = State::Running;
        return Result::Success;
    }

    ConnectionPool::Result ConnectionPool::StopSync()
    {
        std::unique_lock lk{ m_mutex };

        if (m_state != State::Running)
            return Result::StopDenied_NotStarted;

        m_state = State::Stopping;
        m_sessionsCV.notify_all();

        m_acquiredCountZeroCV.wait(lk, [&] {
            return m_acquiredCount == 0;
        });

        SessionTimeoutJob_StopSync();
        m_state = State::Stopped;

        return Result::Success;
    }

    std::expected<ConnectionPool::SessionGuard, ConnectionPool::Result>
    ConnectionPool::Acquire()
    {
        Session *session{ nullptr };
        bool needConnect{ false };

        {
            std::unique_lock lk{ m_mutex };

            m_sessionsCV.wait(lk, [&] {
                if (m_state != State::Running)
                    return true;

                for (auto& s : m_sessions)
                    if (!s.isAcquired)
                    {
                        ++m_acquiredCount;
                        s.isAcquired = true;
                        session     = &s;
                        needConnect = !session->instance;

                        return true;
                    }

                return false;
            });

            if (m_state != State::Running)
                return std::unexpected { Result::AcquireDenied_NotStarted };
        }

        if (needConnect)
        {
            auto newConnection = OpenSessionInstance();

            if (!newConnection)
            {
                Logger::Error() <<
                "[ConnectionPool.Acquire]: Failed to connect to the "
                "MySQL server";

                Release(*session);
                return std::unexpected { Result::AcquireFailed_ConnectionFailed };
            }

            std::unique_lock lk{ m_mutex };
            if (m_state != State::Running)
            {
                Release(*session);
                return std::unexpected { Result::AcquireDenied_NotStarted };
            }

            Logger::Debug() <<
            "[ConnectionPool.Acquire]: Opened a new session with the "
            "MySQL server";

            session->instance.emplace(std::move(*newConnection));
        }

        return SessionGuard(&*session->instance, SessionReleaser{ this, session });
    }

    void ConnectionPool::Release(ConnectionPool::Session& session) noexcept
    {
        std::unique_lock lk{ m_mutex };

        assert(m_acquiredCount != 0);

        --m_acquiredCount;
        session.isAcquired = false;
        session.lastUsed   = Thread::Scheduler::Now();

        m_sessionsCV.notify_one();

        if (m_state == State::Stopping && m_acquiredCount == 0)
            m_acquiredCountZeroCV.notify_one();
    }

    bool ConnectionPool::SessionTimeoutJob_Start()
    {
        {
            std::unique_lock lk{ m_mutex };

            if (m_sessionTimeoutJobState == SessionTimeoutJobState::Running)
                return false;

            m_sessionTimeoutJobState = SessionTimeoutJobState::Running;
        }

        return SessionTimeoutJob_Tick();
    }

    bool ConnectionPool::SessionTimeoutJob_Tick()
    {
        std::unique_lock lk{ m_mutex };

        if (m_sessionTimeoutJobState != SessionTimeoutJobState::Running)
            return false;

        const auto now = Thread::Scheduler::Now();
        auto nextWakeup = now + SessionTimeout;

        for (auto& s : m_sessions)
        {
            if (s.isAcquired || !s.instance)
                continue;

            auto deadline = s.lastUsed + SessionTimeout;

            if (deadline <= now)
            {
                try { s.instance->close(); } catch (...) {}
                s.instance.reset();

                Logger::Debug() <<
                "[ConnectionPool.SessionTimeoutJob_Tick]: Closed a MySQL "
                "session because it wasn't being used";
            }
            else
                nextWakeup = std::min(nextWakeup, deadline);
        }

        lk.unlock();
        auto r = m_scheduler->ScheduleJobAt(nextWakeup, [this] {
            SessionTimeoutJob_Tick();
        }, &m_sessionTimeoutJob);
        lk.lock();

        if (r != Thread::Scheduler::Result::Success)
        {
            Logger::Error() <<
            "[ConnectionPool.SessionTimeoutJob_Tick]: Couldn't "
            "schedule the next timeout job. Error code: " + std::to_string((int)r);

            m_sessionTimeoutJobState = SessionTimeoutJobState::Error;
            return false;
        }

        return true;
    }

    bool ConnectionPool::SessionTimeoutJob_StopSync()
    {
        std::unique_lock lk{ m_mutex };

        if (m_sessionTimeoutJobState != SessionTimeoutJobState::Running)
            return false;

        m_sessionTimeoutJobState = SessionTimeoutJobState::Stopping;

        for (;;)
        {
            auto jobHandle = m_sessionTimeoutJob;

            lk.unlock();
            m_scheduler->CancelJobOrWait(jobHandle);
            lk.lock();

            if (m_sessionTimeoutJob == jobHandle)
                break;
        }

        m_sessionTimeoutJobState = SessionTimeoutJobState::Stopped;
        return true;
    }

    std::optional<mysqlx::Session> ConnectionPool::OpenSessionInstance()
    {
        try
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
        catch (mysqlx::Error& err)
        {
            return std::nullopt;
        }
    }
}