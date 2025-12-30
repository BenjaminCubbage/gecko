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
        EXPECT(usernameValid == Result::Success, usernameValid);

        Result oidcValid = ValidateOIDC(oidcIss, oidcSub);
        EXPECT(oidcValid == Result::Success, oidcValid);

        if (m_dbUsers->CreateUser(username, oidcIss, oidcSub) == DB::UsersTable::Result::Success)
            return Result::Success;

        bool exists{};

        EXPECT(m_dbUsers->OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!exists, Result::UserAlreadyExists);

        bool usernameTaken{};

        EXPECT(m_dbUsers->UsernameExists(username, &usernameTaken) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUserIDByOIDC(const std::string& oidcIss,
                                  const std::string& oidcSub,
                                  int* outUserID)
    {
        if (m_dbUsers->GetUserIDByOIDC(oidcIss, oidcSub, outUserID) == DB::UsersTable::Result::Success)
            return Result::Success;
        
        Result oidcValid = ValidateOIDC(oidcIss, oidcSub);
        EXPECT(oidcValid == Result::Success, oidcValid);

        bool exists{};

        EXPECT(m_dbUsers->OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUser(int userID, Models::User* outUser)
    {
        if (m_dbUsers->GetUser(userID, outUser) == DB::UsersTable::Result::Success)
            return Result::Success;

        bool exists{};
        EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    UsersService::Result 
    UsersService::GetUserByUsername(const std::string& username,
                                    Models::User* outUser)
    {
        bool exists{};

        Result usernameValid = ValidateUsername(username);
        EXPECT(usernameValid == Result::Success, usernameValid);

        EXPECT(m_dbUsers->GetUserByUsernameIfExists(username, &exists, outUser) 
               == DB::UsersTable::Result::Success,
               Result::DatabaseError);

        EXPECT(exists, Result::UserNotFound);
        return Result::Success;
    }

    UsersService::Result
    UsersService::PatchUser(int userID, const Models::UserPatch& patch)
    {
        // todo(ben): Implement propert PATCH
        // Currently, this is effectively a PUT op

        Result usernameValid = ValidateUsername(patch.username);
        EXPECT(usernameValid == Result::Success, usernameValid);

        if (m_dbUsers->PatchUser(userID, patch) == DB::UsersTable::Result::Success)
            return Result::Success;

        bool exists{};

        EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::Success, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        bool usernameTaken{};

        EXPECT(m_dbUsers->UsernameExists(patch.username, &usernameTaken) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }    
}