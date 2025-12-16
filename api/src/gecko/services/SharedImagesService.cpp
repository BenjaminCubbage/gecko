#include "gecko/services/SharedImagesService.h"
#include <iostream>
#include "gecko/rand/UUID.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    SharedImagesService::Result SharedImagesService::CreateSharedImage(int senderID,
                                                                       int receiverID,
                                                                       const std::string& idempotencyKey,
                                                                       const std::vector<uint8_t>& bytes)
    {
        // todo: validate length of bytes

        EXPECT(Rand::UUID::IsValidV4UUID(idempotencyKey), Result::BadIdempotencyKey);
        
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
}