#include "gecko/services/SharedImagesService.h"
#include <iostream>

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    SharedImagesService::Result SharedImagesService::CreateSharedImage(int senderID,
                                                                       int receiverID)
    {
        // TODO: Do this after attempt to send?

        bool senderExists{};
        EXPECT(m_dbUsers.UserExists(senderID, &senderExists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(senderExists, Result::SenderNotFound);
        
        bool receiverExists{};
        EXPECT(m_dbUsers.UserExists(receiverID, &receiverExists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(receiverExists, Result::ReceiverNotFound);
        
        // It is okay for a user to send an image to themselves.

        std::cout << "succ" << std::endl;

        return Result::Success;
    }
}