#pragma once
#include <string>
#include "gecko/db/UsersTable.h"
#include "gecko/models/User.h"
#include "gecko/models/UserPatch.h"

namespace Gecko::API::Services
{
    class SharedImagesService
    {
    public:
        enum class Result
        {
            Success,

            /* Service-space */
            SenderNotFound,
            ReceiverNotFound,

            /* DB fallthrough */
            DatabaseError
        };

        SharedImagesService(DB::UsersTable dbUsers) : m_dbUsers(dbUsers) {}

        Result CreateSharedImage(int senderID,
                                 int receiverID);

    private:
        DB::UsersTable m_dbUsers;
        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local Json::Reader     s_jsonReader;
    };
}