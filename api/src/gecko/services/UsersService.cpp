#include "gecko/services/UsersService.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    thread_local Json::FastWriter UsersService::s_jsonWriter{};
    thread_local Json::Reader     UsersService::s_jsonReader{};

    UsersService::Result
    UsersService::CreateUser(const std::string& username,
                             const std::string& oidcIss,
                             const std::string& oidcSub)
    {
        Result usernameValid = ValidateUsername(username);
        EXPECT(usernameValid == Result::OK, usernameValid);

        Result oidcValid = ValidateOIDC(oidcIss, oidcSub);
        EXPECT(oidcValid == Result::OK, oidcValid);

        if (m_dbUsers->CreateUser(username, oidcIss, oidcSub) == DB::UsersTable::Result::OK)
            return Result::OK;

        bool exists{};

        EXPECT(m_dbUsers->OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::UsersTable::Result::OK,
               Result::DatabaseError);
        EXPECT(!exists, Result::UserAlreadyExists);

        bool usernameTaken{};

        EXPECT(m_dbUsers->UsernameExists(username, &usernameTaken) == DB::UsersTable::Result::OK,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUserIDByOIDC(const std::string& oidcIss,
                                  const std::string& oidcSub,
                                  int* outUserID)
    {
        if (m_dbUsers->GetUserIDByOIDC(oidcIss, oidcSub, outUserID) == DB::UsersTable::Result::OK)
            return Result::OK;

        Result oidcValid = ValidateOIDC(oidcIss, oidcSub);
        EXPECT(oidcValid == Result::OK, oidcValid);

        bool exists{};

        EXPECT(m_dbUsers->OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::UsersTable::Result::OK,
               Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUser(int userID, Models::User* outUser)
    {
        if (m_dbUsers->GetUser(userID, outUser) == DB::UsersTable::Result::OK)
            return Result::OK;

        bool exists{};
        EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::OK, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUserByUsername(const std::string& username,
                                    Models::User* outUser)
    {
        bool exists{};

        Result usernameValid = ValidateUsername(username);
        EXPECT(usernameValid == Result::OK, usernameValid);

        EXPECT(m_dbUsers->GetUserByUsernameIfExists(username, &exists, outUser)
               == DB::UsersTable::Result::OK,
               Result::DatabaseError);

        EXPECT(exists, Result::UserNotFound);
        return Result::OK;
    }

    UsersService::Result
    UsersService::PatchUser(int userID, const Models::UserPatch& patch)
    {
        // todo(ben): Implement propert PATCH
        // Currently, this is effectively a PUT op

        Result usernameValid = ValidateUsername(patch.username);
        EXPECT(usernameValid == Result::OK, usernameValid);

        if (m_dbUsers->PatchUser(userID, patch) == DB::UsersTable::Result::OK)
            return Result::OK;

        bool exists{};

        EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::OK, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        bool usernameTaken{};

        EXPECT(m_dbUsers->UsernameExists(patch.username, &usernameTaken) == DB::UsersTable::Result::OK,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }
}