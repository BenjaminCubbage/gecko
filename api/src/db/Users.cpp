#include "gecko/db/Users.h"

namespace Gecko::API::DB
{
    Users::Result Users::UsernameExists(const std::string& username, bool* outTaken)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql("SELECT user_id FROM Users WHERE username=?")
                    .bind(username)
                    .execute();

            *outTaken = result.count() > 0;
            return Result::Success;
        }
        catch (mysqlx::Error&) 
        { 
            return Result::Failure; 
        }
    }

    Users::Result Users::CreateUser(const std::string& username, 
                                    const std::string& oidcIss, 
                                    const std::string& oidcSub)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            const auto result =
                connection->sql("INSERT INTO Users (username, oidc_iss, oidc_sub) "
                                "VALUES (?, ?, ?) ")
                    .bind(username, oidcIss, oidcSub)
                    .execute();

            return Result::Success;
        }
        catch (mysqlx::Error&) 
        { 
            return Result::Failure; 
        }
    }
    
    Users::Result Users::OIDCIdentityExists(const std::string& oidcIss,
                                            const std::string& oidcSub,
                                            bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result = 
                connection->sql("SELECT user_id FROM Users "
                                "WHERE oidc_iss=? AND oidc_sub=?")
                    .bind(oidcIss, oidcSub)
                    .execute();

            *outExists = result.hasData() && result.count() > 0;
            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    Users::Result Users::UserExists(int userID, bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result = 
                connection->sql("SELECT user_id FROM Users "
                                "WHERE user_id=?")
                    .bind(userID)
                    .execute();

            *outExists = result.hasData() && result.count() != 1;
            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    Users::Result Users::GetUserIDByOIDC(const std::string& oidcIss,
                                         const std::string& oidcSub,
                                         int* outUserID)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result = 
                connection->sql("SELECT user_id FROM Users "
                                "WHERE oidc_iss=? AND oidc_sub=?")
                    .bind(oidcIss, oidcSub)
                    .execute();

            if (!result.hasData() || result.count() != 1)
                return Result::Failure;

            *outUserID = result.fetchOne().get(0).get<int>();
            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
    
    Users::Result Users::GetUser(int userID, Models::User* outUser)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();
            
            auto result =
                connection->sql("SELECT user_id, username, oidc_iss, oidc_sub FROM Users "
                                "WHERE user_id=?")
                    .bind(userID)
                    .execute();

            if (!result.hasData() || result.count() != 1)
                return Result::Failure;

            auto columns = result.fetchOne();

            *outUser = Models::User
            {
                .userID   = columns.get(0).get<int>(),
                .username = columns.get(1).get<std::string>(),
                .oidcIss  = columns.get(2).get<std::string>(),
                .oidcSub  = columns.get(3).get<std::string>()
            };

            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
    
    Users::Result Users::PatchUser(int userID, const Models::UserPatch& patch)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();
            
            auto result = 
                connection->sql("UPDATE Users SET username=? WHERE user_id=?")
                    .bind(patch.username, userID)
                    .execute();
            
            return result.getAffectedItemsCount()
                ? Result::Success
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}