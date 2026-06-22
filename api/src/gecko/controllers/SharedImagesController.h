#pragma once
#include <string>
#include "httplib.h"
#include "gecko/services/SharedImagesService.h"

namespace Gecko::API::Controllers
{
    class SharedImagesController
    {
      public:
        SharedImagesController(Services::SharedImagesService sharedImagesService, std::string pubkey) :
                               m_sharedImagesService(sharedImagesService),
                               m_pubkey(std::move(pubkey)) {}

        void Attach(httplib::Server& server);

      private:
        void Handle_POST_SharedImages(const httplib::Request& req,
                                      httplib::Response& res,
                                      const httplib::ContentReader& contentReader);

        Services::SharedImagesService m_sharedImagesService;
        std::string m_pubkey;
    };
}