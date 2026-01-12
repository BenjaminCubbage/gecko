#include "gecko/db/UsersTable.h"

namespace Gecko::API::DB
{
    using DB::ConnectionPool;

    UsersTable::Result UsersTable::UsernameExists(const std::string& username, bool* outTaken)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT 1 FROM Users WHERE username=?")
                    .bind(username)
                    .execute();

            *outTaken = result.count() > 0;
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::CreateUser(const std::string& username,
                                              const std::string& oidcIss,
                                              const std::string& oidcSub)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            const auto result =
                connection.value()->sql("INSERT INTO Users (username, oidc_iss, oidc_sub) "
                                "VALUES (?, ?, ?) ")
                    .bind(username, oidcIss, oidcSub)
                    .execute();

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::OIDCIdentityExists(const std::string& oidcIss,
                                                      const std::string& oidcSub,
                                                      bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT user_id FROM Users "
                                "WHERE oidc_iss=? AND oidc_sub=?")
                    .bind(oidcIss, oidcSub)
                    .execute();

            *outExists = result.count() > 0;
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::UserExists(int userID, bool *outExists)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT user_id FROM Users "
                                "WHERE user_id=?")
                    .bind(userID)
                    .execute();

            *outExists = result.count() == 1;
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::GetUser(int userID, Models::User* outUser)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT user_id, username, oidc_iss, oidc_sub FROM Users "
                                "WHERE user_id=?")
                    .bind(userID)
                    .execute();

            if (!result.hasData() || result.count() != 1)
                return Result::Failure;

            auto columns = result.fetchOne();

            *outUser = {
                .userID   = columns.get(0).get<int>(),
                .username = columns.get(1).get<std::string>()
            };

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::GetUserIDByOIDC(const std::string& oidcIss,
                                                   const std::string& oidcSub,
                                                   int* outUserID)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT user_id FROM Users "
                                "WHERE oidc_iss=? AND oidc_sub=?")
                    .bind(oidcIss, oidcSub)
                    .execute();

            if (!result.hasData() || result.count() != 1)
                return Result::Failure;

            *outUserID = result.fetchOne().get(0).get<int>();
            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result
    UsersTable::GetUserByUsernameIfExists(const std::string& username,
                                          bool* outExists,
                                          Models::User* outUser)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("SELECT user_id, username FROM Users WHERE username=?")
                    .bind(username)
                    .execute();

            if (!result.hasData() || result.count() == 0)
            {
                *outExists = false;
                *outUser   = {};
                return Result::OK;
            }

            auto row = result.fetchOne();

            *outExists = true;
            *outUser = {
                .userID   = row.get(0).get<int>(),
                .username = row.get(1).get<std::string>()
            };

            return Result::OK;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    UsersTable::Result UsersTable::PatchUser(int userID, const Models::UserPatch& patch)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            if (!connection)
                return Result::Failure;

            auto result =
                connection.value()->sql("UPDATE Users SET username=? WHERE user_id=?")
                    .bind(patch.username, userID)
                    .execute();

            return result.getAffectedItemsCount()
                ? Result::OK
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}