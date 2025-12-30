#pragma once
#include <ostream>
#include "gecko/env/Env.h"
#include "gecko/server/Controllers.h"
#include "gecko/server/Services.h"
#include "gecko/server/Tables.h"
#include "gecko/server/Topics.h"
#include "httplib.h"

namespace Gecko::API::Server
{
    class Server
    {
    public:
        Server(Env::Env env, std::ostream* log)
            : m_env{ std::move(env) },
              m_log{ log },
              m_topics     { &m_env, m_log },
              m_tables     { &m_env, m_log },
              m_services   { &m_env, m_log, &m_tables, &m_topics },
              m_controllers{ &m_env, m_log, &m_services },
              m_httpServer{
                  m_env.geckoAPITLSCertPath.c_str(),
                  m_env.geckoAPITLSPkeyPath.c_str() } {}

        Server           (const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server           (Server&&) = delete;
        Server& operator=(Server&&) = delete;

        bool Start();

    private:
        Env::Env      m_env;
        std::ostream* m_log;

        Topics      m_topics;
        Tables      m_tables;
        Services    m_services;
        Controllers m_controllers;
        
        httplib::SSLServer m_httpServer;
    };
}