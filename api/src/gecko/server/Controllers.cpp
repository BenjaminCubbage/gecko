#include "gecko/server/Controllers.h"

namespace Gecko::API::Server
{
    bool Controllers::Start(httplib::Server& server)
    {
        m_corsPolicy.Attach(server);

        m_authController        .Attach(server);
        m_usersController       .Attach(server);
        m_sharedImagesController.Attach(server);
        m_devicesController     .Attach(server);
        m_friendshipsController .Attach(server);
        return true;
    }
}