#pragma once
#include <string>
#include <vector>
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/services/DevicesService.h"
#include "gecko/services/FriendshipsService.h"
#include "gecko/topics/LatestImageTopic.h"

namespace Gecko::API::Services
{
    class SharedImagesService
    {
      public:
        static constexpr int MaxImageSize = 1024 * 10;

        enum class Result
        {
            OK,

            /* Service-space */
            BadIdempotencyKey,
            ImageTooLarge,

            /* DB-space */
            DeviceNotFound,
            IdempotencyKeyReplayed,
            NotFriends,
            ReceiverNotFound,
            SenderNotFound,
            
            /* Fallthrough cases */
            DatabaseError,
            DeviceServiceError,
            FriendshipsServiceError
        };

        SharedImagesService(Services::DevicesService* devicesService,
                            Services::FriendshipsService* friendshipsService,
                            Topics::LatestImageTopic* latestImageTopic,
                            DB::SharedImagesTable* dbSharedImages,
                            DB::UsersTable* dbUsers)
            : m_devicesService(devicesService),
              m_friendshipsService(friendshipsService),
              m_dbSharedImages(dbSharedImages),
              m_dbUsers(dbUsers),
              m_latestImageTopic(latestImageTopic) {}

        Result CreateSharedImage(int senderUserID,
                                 int recipientDeviceID,
                                 const std::string& idempotencyKey,
                                 const std::vector<uint8_t>& bytes);

        Result GetLatestReceivedImageBlob(int recipientUserID,
                                          std::vector<uint8_t>* outBlob);

      private:
        Services::DevicesService*     m_devicesService;
        Services::FriendshipsService* m_friendshipsService;

        Topics::LatestImageTopic* m_latestImageTopic;

        DB::SharedImagesTable* m_dbSharedImages;
        DB::UsersTable*        m_dbUsers;

        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local Json::Reader     s_jsonReader;
    };
}