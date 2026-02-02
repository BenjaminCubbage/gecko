#include "gecko/services/SharedImagesService.h"
#include <algorithm>
#include "gecko/models/Device.h"
#include "gecko/models/FriendshipMetadata.h"
#include "gecko/models/SharedImage.h"
#include "gecko/models/User.h"
#include "gecko/rand/UUID.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    SharedImagesService::Result
    SharedImagesService::CreateSharedImage(int senderUserID,
                                           int recipientDeviceID,
                                           const std::string& idempotencyKey,
                                           const std::vector<uint8_t>& bytes)
    {
        /*
            Sanity checks
        */
        EXPECT(Rand::UUID::IsValidV4UUID(idempotencyKey), Result::BadIdempotencyKey);
        EXPECT(bytes.size() < MaxImageSize, Result::ImageTooLarge);

        /*
            Idempotency key not already played?
        */
        bool idempotencyKeyExists{};
        EXPECT(m_dbSharedImages->IdempotencyKeyExists(idempotencyKey, &idempotencyKeyExists)
               == DB::SharedImagesTable::Result::OK, Result::DatabaseError);
        EXPECT(!idempotencyKeyExists, Result::IdempotencyKeyReplayed);

        /*
            Device exists?
        */
        bool deviceExists{};
        int recipientUserID{};
        switch (m_devicesService->GetDeviceExists(
            recipientDeviceID,
            &deviceExists,
            &recipientUserID))
        {
            case DevicesService::Result::OK:
                break;

            case DevicesService::Result::DatabaseError:
                return Result::DatabaseError;

            default:
                return Result::DeviceServiceError;
        }
        EXPECT(deviceExists, Result::ReceiverNotFound);

        /*
            Sender and recipient friends?
        */
        if (senderUserID != recipientUserID)
        {
            bool areFriends{};
            switch(m_friendshipsService->FriendshipExists(
                senderUserID,
                recipientUserID,
                &areFriends))
            {
                case FriendshipsService::Result::OK:
                case FriendshipsService::Result::SelfFriendNotAllowed:
                    break;

                case FriendshipsService::Result::User1NotFound:
                    return Result::SenderNotFound;

                case FriendshipsService::Result::User2NotFound:
                    return Result::ReceiverNotFound;

                default:
                    return Result::FriendshipsServiceError;
            }
            EXPECT(areFriends, Result::NotFriends);
        }

        /*
            Send image
        */
        if (int sharedImageID{};
            m_dbSharedImages->CreateSharedImage(
                senderUserID,
                recipientDeviceID,
                idempotencyKey,
                bytes,
                &sharedImageID) == DB::SharedImagesTable::Result::OK)
        {
            m_latestImageTopic->PublishLatestImage(
                recipientDeviceID,
                sharedImageID,
                bytes);

            return Result::OK;
        }

        return Result::DatabaseError;
    }

    SharedImagesService::Result
    SharedImagesService::GetLatestReceivedImageBlob(int recipientUserID,
                                                    std::vector<uint8_t>* outBlob)
    {
        if (m_dbSharedImages->GetLatestReceivedImageBlob(recipientUserID, outBlob) == DB::SharedImagesTable::Result::OK)
            return Result::OK;

        bool receiverExists{};
        EXPECT(m_dbUsers->UserExists(recipientUserID, &receiverExists) == DB::UsersTable::Result::OK, Result::DatabaseError);
        EXPECT(receiverExists, Result::ReceiverNotFound);

        return Result::DatabaseError;
    }
}