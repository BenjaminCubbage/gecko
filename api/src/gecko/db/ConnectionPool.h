#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <expected>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <condition_variable>
#include "mysqlx/xdevapi.h"
#include "gecko/thread/Scheduler.h"

namespace Gecko::API::DB
{
    class ConnectionPool
    {
    public:
        static constexpr const size_t PoolSize = 4;
        static constexpr const std::chrono::minutes SessionTimeout{ 1 };

        enum class Result
        {
            Success = 0,

            StartDenied_NotIdle,
            StartFailed_CouldNotStartTimeoutJob,

            StopDenied_NotStarted,

            AcquireDenied_NotStarted,
            AcquireFailed_ConnectionFailed
        };

    private:
        enum class State
        {
            Stopped, Starting, Running, Stopping
        };

        enum class SessionTimeoutJobState
        {
            Stopped, Running, Stopping, Error
        };

        struct Session
        {
            bool isAcquired{ false };
            std::optional<mysqlx::Session> instance{ std::nullopt };
            Thread::Scheduler::TimePoint lastUsed{};
        };

        struct SessionReleaser
        {
            SessionReleaser(ConnectionPool* pool, Session* session)
                : m_pool(pool), m_session(session) {}

            ConnectionPool* m_pool;
            Session* m_session;

            void operator()(mysqlx::Session*) const noexcept {
                m_pool->Release(*m_session);
            }
        };

        typedef std::unique_ptr<mysqlx::Session, SessionReleaser> SessionGuard;

    public:
        ConnectionPool(Thread::Scheduler* scheduler,
                       std::string host,
                       int port,
                       std::string user,
                       std::string pwd,
                       std::string db)
            : m_scheduler(scheduler),
              m_host(std::move(host)),
              m_port(port),
              m_user(std::move(user)),
              m_pwd(std::move(pwd)),
              m_db(std::move(db)) { }

        ~ConnectionPool()
        {
            (void)StopSync();
        }

        Result Start();
        Result StopSync();

        [[nodiscard]] std::expected<SessionGuard, Result> Acquire();

    private:
        void Release(Session& session) noexcept;

        bool SessionTimeoutJob_Start();
        bool SessionTimeoutJob_Tick();
        bool SessionTimeoutJob_StopSync();

        inline std::optional<mysqlx::Session> OpenSessionInstance();

        std::mutex m_mutex;
        State m_state{ State::Stopped };

        std::array<Session, PoolSize> m_sessions;
        std::condition_variable m_sessionsCV;

        std::atomic<size_t> m_acquiredCount;
        std::condition_variable m_acquiredCountZeroCV;

        std::string m_host;
        int         m_port;
        std::string m_user;
        std::string m_pwd;
        std::string m_db;

        Thread::Scheduler* m_scheduler;
        Thread::Scheduler::TaskHandle m_sessionTimeoutJob;
        SessionTimeoutJobState m_sessionTimeoutJobState{ SessionTimeoutJobState::Stopped };
    };
}