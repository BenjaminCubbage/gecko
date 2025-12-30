#pragma once
#include <ostream>
#include "gecko/env/Env.h"
#include "gecko/server/Tables.h"
#include "gecko/server/Topics.h"
#include "gecko/services/DevicesService.h"
#include "gecko/services/FriendshipsService.h"
#include "gecko/services/SharedImagesService.h"
#include "gecko/services/UsersService.h"

namespace Gecko::API::Server
{
    class Services
    {
    public:
        Services(Env::Env* env, std::ostream* log, Tables* tables, Topics* topics)
            : m_env{ env }, 
              m_log{ log }, 
              m_tables{ tables },
              m_usersService       { &tables->Users() },
              m_sharedImagesService{ &tables->SharedImages(), &tables->Users() },
              m_devicesService     { &topics->Heartbeat(), &tables->Devices(), &tables->Users() },
              m_friendshipsService { &tables->Friendships(), &tables->Users() } {}
              
        Services           (const Services&) = delete;
        Services& operator=(const Services&) = delete;
        Services           (Services&&) = delete;
        Services& operator=(Services&&) = delete;
        
        inline API::Services::UsersService&        Users()        { return m_usersService; }
        inline API::Services::SharedImagesService& SharedImages() { return m_sharedImagesService; }
        inline API::Services::DevicesService&      Devices()      { return m_devicesService; }
        inline API::Services::FriendshipsService&  Friendships()  { return m_friendshipsService; }

    private:
        Env::Env* m_env;
        std::ostream* m_log;
        Topics* m_topics;
        Tables* m_tables;

        API::Services::UsersService        m_usersService;
        API::Services::SharedImagesService m_sharedImagesService;
        API::Services::DevicesService      m_devicesService;
        API::Services::FriendshipsService  m_friendshipsService;
    };
}