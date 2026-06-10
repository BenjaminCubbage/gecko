#pragma once
#include <ostream>
#include "gecko/controllers/AuthController.h"
#include "gecko/controllers/DevicesController.h"
#include "gecko/controllers/FriendshipsController.h"
#include "gecko/controllers/SharedImagesController.h"
#include "gecko/controllers/UsersController.h"
#include "gecko/env/Env.h"
#include "gecko/http_policy/CORSPolicy.h"
#include "gecko/server/Services.h"
#include "httplib.h"

namespace Gecko::API::Server
{
    class Controllers
    {
      public:
        Controllers(Env::Env* env, Services* services)
            : m_env{ env },
              m_services{ services },
              m_authController{
                m_services->Users(),
                m_env->oauthClientID,
                m_env->oauthClientSecret,
                m_env->jwtPrivateKey,
                m_env->jwtPublicKey },
              m_corsPolicy            { "https://localhost:3000" },
              m_usersController       { m_services->Users(),        m_env->jwtPublicKey },
              m_sharedImagesController{ m_services->SharedImages(), m_env->jwtPublicKey },
              m_devicesController     { m_services->Devices(),      m_env->jwtPublicKey },
              m_friendshipsController { m_services->Friendships(),  m_env->jwtPublicKey } {}

        Controllers           (const Controllers&) = delete;
        Controllers& operator=(const Controllers&) = delete;
        Controllers           (Controllers&&) = delete;
        Controllers& operator=(Controllers&&) = delete;

        bool Start(httplib::Server& server);

      private:
        Env::Env* m_env;
        Services* m_services;

        API::HttpPolicy::CORSPolicy m_corsPolicy;

        API::Controllers::AuthController         m_authController;
        API::Controllers::UsersController        m_usersController;
        API::Controllers::SharedImagesController m_sharedImagesController;
        API::Controllers::DevicesController      m_devicesController;
        API::Controllers::FriendshipsController  m_friendshipsController;
    };
}