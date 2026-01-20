#pragma once
#include "gecko/env/Env.h"
#include "gecko/server/Controllers.h"
#include "gecko/server/Services.h"
#include "gecko/server/Tables.h"
#include "gecko/server/Topics.h"
#include "gecko/thread/ThreadPool.h"
#include "gecko/thread/Scheduler.h"
#include "httplib.h"

namespace Gecko::API::Server
{
    class Server
    {
    public:
        Server(Env::Env env)
            : m_env{ std::move(env) },
              m_threadPool {},
              m_scheduler  { &m_threadPool },
              m_topics     { &m_env },
              m_tables     { &m_env, &m_scheduler },
              m_services   { &m_env, &m_tables, &m_topics },
              m_controllers{ &m_env, &m_services },
              m_httpServer{
                  m_env.geckoAPITLSCertPath.c_str(),
                  m_env.geckoAPITLSPkeyPath.c_str() }
        {
            m_httpServer.set_tcp_nodelay(true);
            m_httpServer.set_payload_max_length(16 * 1024);
        }

        Server           (const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server           (Server&&) = delete;
        Server& operator=(Server&&) = delete;

        bool Start();

    private:
        Env::Env      m_env;

        Thread::ThreadPool m_threadPool;
        Thread::Scheduler  m_scheduler;

        Topics      m_topics;
        Tables      m_tables;
        Services    m_services;
        Controllers m_controllers;

        httplib::SSLServer m_httpServer;
    };
}