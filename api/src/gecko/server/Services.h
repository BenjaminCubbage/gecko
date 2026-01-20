#pragma once
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
        Services(Env::Env* env, Tables* tables, Topics* topics)
            : m_env{ env },
              m_tables{ tables },
              m_usersService       { &tables->Users() },
              m_devicesService     { &topics->Heartbeat(), &tables->Devices(), &tables->Users() },
              m_sharedImagesService{ &m_devicesService, &topics->LatestImage(), &tables->SharedImages(), &tables->Users() },
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

        Topics* m_topics;
        Tables* m_tables;

        API::Services::UsersService        m_usersService;
        API::Services::DevicesService      m_devicesService;
        API::Services::SharedImagesService m_sharedImagesService;
        API::Services::FriendshipsService  m_friendshipsService;
    };
}