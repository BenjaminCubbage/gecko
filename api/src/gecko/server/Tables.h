#pragma once
#include <ostream>
#include "gecko/db/DevicesTable.h"
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/env/Env.h"

namespace Gecko::API::Server
{
    class Tables
    {
    public:
        Tables(Env::Env* env, std::ostream* log)
            : m_env{ env },
              m_log{ log },
              m_dbConnectionPool{
                "127.0.0.1",
                m_env->mysqlXAPIPort, "root",
                m_env->mysqlPassword, "Gecko" },
            m_dbUsers       { &m_dbConnectionPool },
            m_dbSharedImages{ &m_dbConnectionPool },
            m_dbDevices     { &m_dbConnectionPool } {}
              
        Tables           (const Tables&) = delete;
        Tables& operator=(const Tables&) = delete;
        Tables           (Tables&&) = delete;
        Tables& operator=(Tables&&) = delete;

        DB::UsersTable&        Users()        { return m_dbUsers; }
        DB::SharedImagesTable& SharedImages() { return m_dbSharedImages; }
        DB::DevicesTable&      Devices()      { return m_dbDevices; }

        bool Start();

    private:
        Env::Env *m_env;
        std::ostream* m_log;

        DB::ConnectionPool    m_dbConnectionPool;
        DB::UsersTable        m_dbUsers;
        DB::SharedImagesTable m_dbSharedImages;
        DB::DevicesTable      m_dbDevices;
    };
}