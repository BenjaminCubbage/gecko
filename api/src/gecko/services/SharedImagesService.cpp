#include "gecko/services/SharedImagesService.h"
#include <iostream>
#include <type_traits>
#include "gecko/rand/UUID.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    SharedImagesService::Result 
    SharedImagesService::CreateSharedImage(int senderID,
                                           int receiverID,
                                           const std::string& idempotencyKey,
                                           const std::vector<uint8_t>& bytes)
    {
        EXPECT(Rand::UUID::IsValidV4UUID(idempotencyKey), Result::BadIdempotencyKey);
        EXPECT(bytes.size() < MaxImageSize, Result::ImageTooLarge);
        
        bool idempotencyKeyExists{};
        EXPECT(m_dbSharedImages.IdempotencyKeyExists(idempotencyKey, &idempotencyKeyExists) 
               == DB::SharedImagesTable::Result::Success, Result::DatabaseError);
        EXPECT(!idempotencyKeyExists, Result::IdempotencyKeyReplayed);

        if (m_dbSharedImages.CreateSharedImage(senderID, receiverID, idempotencyKey, bytes) == DB::SharedImagesTable::Result::Success)
            return Result::Success;

        bool senderExists{};
        EXPECT(m_dbUsers.UserExists(senderID, &senderExists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(senderExists, Result::SenderNotFound);
        
        bool receiverExists{};
        EXPECT(m_dbUsers.UserExists(receiverID, &receiverExists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(receiverExists, Result::ReceiverNotFound);

        return Result::DatabaseError;
    }

    SharedImagesService::Result 
    SharedImagesService::GetLatestReceivedImageBlob(int receiverID,
                                                    std::vector<uint8_t>* outBlob)
    {
        if (m_dbSharedImages.GetLatestReceivedImageBlob(receiverID, outBlob) == DB::SharedImagesTable::Result::Success)
            return Result::Success;

        bool receiverExists{};
        EXPECT(m_dbUsers.UserExists(receiverID, &receiverExists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(receiverExists, Result::ReceiverNotFound);

        return Result::DatabaseError;
    }
}