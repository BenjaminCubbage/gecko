#pragma once
#include <string>
#include "gecko/db/UsersTable.h"
#include "gecko/models/User.h"
#include "gecko/models/UserPatch.h"

namespace Gecko::API::Services
{
    class UsersService
    {
    public:
        static constexpr size_t MinUsernameLength = 3;
        static constexpr size_t MaxUsernameLength = 18;

        static constexpr size_t MinOIDCIssLength = 3;
        static constexpr size_t MaxOIDCIssLength = 36;

        static constexpr size_t MinOIDCSubLength = 3;
        static constexpr size_t MaxOIDCSubLength = 36;

        enum class Result
        {
            Success,

            /* Service-space */
            UsernameTooShort,
            UsernameTooLong,
            OIDCIssTooShort,
            OIDCIssTooLong,
            OIDCSubTooShort,
            OIDCSubTooLong,

            /* DB-space */
            UsernameTaken,
            UserNotFound,
            UserAlreadyExists,

            /* DB fallthrough */
            DatabaseError
        };

        UsersService(DB::UsersTable* dbUsers) : m_dbUsers(dbUsers) {}

        Result CreateUser(const std::string& username,
                          const std::string& oidcIss,
                          const std::string& oidcSub);

        Result GetUserIDByOIDC(const std::string& oidcIss,
                               const std::string& oidcSub,
                               int* outUserID);

        Result GetUser(int userID, Models::User* outUser);

        Result PatchUser(int userID, const Models::UserPatch& patch);

    private:
        DB::UsersTable* m_dbUsers;
        static thread_local Json::FastWriter s_jsonWriter;
        static thread_local Json::Reader     s_jsonReader;
    };
}