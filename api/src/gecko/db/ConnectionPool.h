#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <condition_variable>
#include "mysqlx/xdevapi.h"

namespace Gecko::API::DB
{
    class ConnectionPool
    {
    private:
        struct Session
        {
            Session(mysqlx::Session&& instance) 
                : m_instance(std::move(instance)) {}

            bool m_inUse{ false };
            mysqlx::Session m_instance;
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

        using SessionGuard = std::unique_ptr<mysqlx::Session, SessionReleaser>;

    public:
        // [!!!] Throws on failure
        ConnectionPool(std::string host, int port, std::string user, std::string pwd, std::string db)
            : m_host(std::move(host)), 
              m_port(port), 
              m_user(std::move(user)), 
              m_pwd(std::move(pwd)), 
              m_db(std::move(db)) { }

        bool Connect();

        [[nodiscard]] SessionGuard Acquire();

    private:
        void Release(Session& session) noexcept;
        
        static constexpr size_t PoolSize = 4;

        std::mutex m_sessionsMutex;
        std::vector<Session> m_sessions;
        std::condition_variable m_sessionsSignal;

        std::string m_host;
        int         m_port;
        std::string m_user;
        std::string m_pwd;
        std::string m_db;
    };
}