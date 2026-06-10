#pragma once
#include "gecko/db/DevicesTable.h"
#include "gecko/db/FriendshipsTable.h"
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/env/Env.h"
#include "gecko/thread/ThreadPool.h"
#include "gecko/thread/Scheduler.h"

namespace Gecko::API::Server
{
    class Tables
    {
      public:
        Tables(Env::Env* env, Thread::Scheduler* scheduler)
            : m_env{ env },
              m_scheduler{ scheduler },
              m_dbConnectionPool{
                m_scheduler,
                "127.0.0.1",
                m_env->mysqlXAPIPort, "root",
                m_env->mysqlPassword, "Gecko" },
              m_dbUsers       { &m_dbConnectionPool },
              m_dbSharedImages{ &m_dbConnectionPool },
              m_dbDevices     { &m_dbConnectionPool },
              m_dbFriendships { &m_dbConnectionPool } {}

        Tables           (const Tables&) = delete;
        Tables& operator=(const Tables&) = delete;
        Tables           (Tables&&) = delete;
        Tables& operator=(Tables&&) = delete;

        inline DB::UsersTable&        Users()        { return m_dbUsers; }
        inline DB::SharedImagesTable& SharedImages() { return m_dbSharedImages; }
        inline DB::DevicesTable&      Devices()      { return m_dbDevices; }
        inline DB::FriendshipsTable&  Friendships()  { return m_dbFriendships; }

        bool Start();

      private:
        Env::Env*          m_env;
        Thread::Scheduler* m_scheduler;

        DB::ConnectionPool    m_dbConnectionPool;
        DB::UsersTable        m_dbUsers;
        DB::SharedImagesTable m_dbSharedImages;
        DB::DevicesTable      m_dbDevices;
        DB::FriendshipsTable  m_dbFriendships;
    };
}