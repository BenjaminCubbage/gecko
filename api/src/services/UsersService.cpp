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
        EXPECT(username.size() >= MinUsernameLength, Result::UsernameTooShort);
        EXPECT(username.size() <= MaxUsernameLength, Result::UsernameTooLong);

        EXPECT(oidcIss.size() >= MinOIDCIssLength, Result::OIDCIssTooShort);
        EXPECT(oidcIss.size() <= MaxOIDCIssLength, Result::OIDCIssTooLong);

        EXPECT(oidcSub.size() >= MinOIDCSubLength, Result::OIDCSubTooShort);
        EXPECT(oidcSub.size() <= MaxOIDCSubLength, Result::OIDCSubTooLong);

        if (m_dbUsers.CreateUser(username, oidcIss, oidcSub) == DB::Users::Result::Success)
            return Result::Success;

        bool exists{};

        EXPECT(m_dbUsers.OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::Users::Result::Success, 
               Result::DatabaseError);
        EXPECT(!exists, Result::UserAlreadyExists);

        bool usernameTaken{};
        
        EXPECT(m_dbUsers.UsernameExists(username, &usernameTaken) == DB::Users::Result::Success,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::GetUserIDByOIDC(const std::string& oidcIss, 
                                  const std::string& oidcSub,
                                  int* outUserID)
    {
        if (m_dbUsers.GetUserIDByOIDC(oidcIss, oidcSub, outUserID) == DB::Users::Result::Success)
            return Result::Success;

        bool exists{};

        EXPECT(m_dbUsers.OIDCIdentityExists(oidcIss, oidcSub, &exists) == DB::Users::Result::Success,
               Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }
    
    UsersService::Result
    UsersService::GetUser(int userID, Models::User* outUser)
    {
        if (m_dbUsers.GetUser(userID, outUser) == DB::Users::Result::Success)
            return Result::Success;

        bool exists{};

        EXPECT(m_dbUsers.UserExists(userID, &exists) == DB::Users::Result::Success, Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    UsersService::Result
    UsersService::PatchUser(int userID, const Models::UserPatch& patch)
    {
        // TODO: Implement propert PATCH
        // Currently, all PATCH ops are effectively PUT ops

        EXPECT(patch.username.size() >= MinUsernameLength, Result::UsernameTooShort);
        EXPECT(patch.username.size() <= MaxUsernameLength, Result::UsernameTooLong);

        // TODO: Make sure usernames contain only accepted alphanumeric characters
        
        if (m_dbUsers.PatchUser(userID, patch) == DB::Users::Result::Success)
            return {};

        bool exists{};

        EXPECT(m_dbUsers.UserExists(userID, &exists) == DB::Users::Result::Success, Result::DatabaseError);
        EXPECT(!exists, Result::UserNotFound);

        bool usernameTaken{};
        
        EXPECT(m_dbUsers.UsernameExists(patch.username, &usernameTaken) == DB::Users::Result::Success,
               Result::DatabaseError);
        EXPECT(!usernameTaken, Result::UsernameTaken);

        return Result::DatabaseError;
    }
}