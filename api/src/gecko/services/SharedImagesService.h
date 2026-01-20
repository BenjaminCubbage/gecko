#pragma once
#include <string>
#include <vector>
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/models/SharedImage.h"
#include "gecko/services/DevicesService.h"
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
            SenderNotFound,
            ReceiverNotFound,
            BadIdempotencyKey,
            IdempotencyKeyReplayed,
            ImageTooLarge,

            /* DB fallthrough */
            DatabaseError
        };

        SharedImagesService(Services::DevicesService* devicesService,
                            Topics::LatestImageTopic* latestImageTopic,
                            DB::SharedImagesTable* dbSharedImages,
                            DB::UsersTable* dbUsers)
            : m_devicesService(devicesService),
              m_dbSharedImages(dbSharedImages),
              m_dbUsers(dbUsers),
              m_latestImageTopic(latestImageTopic) {}

        Result CreateSharedImage(int senderID,
                                 int receiverID,
                                 const std::string& idempotencyKey,
                                 const std::vector<uint8_t>& bytes);

        Result GetLatestReceivedImageBlob(int receiverID,
                                          std::vector<uint8_t>* outBlob);

    private:
        Services::DevicesService* m_devicesService;

        Topics::LatestImageTopic* m_latestImageTopic;

        DB::SharedImagesTable* m_dbSharedImages;
        DB::UsersTable* m_dbUsers;

        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local Json::Reader     s_jsonReader;
    };
}