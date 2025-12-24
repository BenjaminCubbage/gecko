#pragma once
#include <string>
#include <vector>
#include "gecko/db/SharedImagesTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/models/SharedImage.h"

namespace Gecko::API::Services
{
    class SharedImagesService
    {
    public:
        static constexpr int MaxImageSize = 10000;

        enum class Result
        {
            Success,

            /* Service-space */
            SenderNotFound,
            ReceiverNotFound,
            BadIdempotencyKey,
            IdempotencyKeyReplayed,
            ImageTooLarge,

            /* DB fallthrough */
            DatabaseError
        };

        SharedImagesService(DB::SharedImagesTable* dbSharedImages, DB::UsersTable* dbUsers) 
            : m_dbSharedImages(dbSharedImages), m_dbUsers(dbUsers) {}

        Result CreateSharedImage(int senderID,
                                 int receiverID,
                                 const std::string& idempotencyKey,
                                 const std::vector<uint8_t>& bytes);

        Result GetLatestReceivedImageBlob(int receiverID,
                                          std::vector<uint8_t>* outBlob);

    private:
        DB::SharedImagesTable* m_dbSharedImages;
        DB::UsersTable* m_dbUsers;
        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local Json::Reader     s_jsonReader;
    };
}