#pragma once
#include <string>
#include "gecko/db/ConnectionPool.h"
#include "gecko/models/User.h"
#include "gecko/models/UserPatch.h"
#include "mysqlx/xdevapi.h"

namespace Gecko::API::DB
{
    class UsersTable
    {
    public:
        enum class Result { Success, Failure };

        UsersTable(std::shared_ptr<ConnectionPool>& connectionPool) : m_connectionPool(connectionPool) {}

        Result UsernameExists(const std::string& username, bool* outTaken);

        Result CreateUser(const std::string& username,
                          const std::string& oidcIss,
                          const std::string& oidcSub);

        Result OIDCIdentityExists(const std::string& oidcIss,
                                  const std::string& oidcSub,
                                  bool *outExists);

        Result UserExists(int userID, bool *outExists);

        Result GetUserIDByOIDC(const std::string& oidcIss,
                               const std::string& oidcSub,
                               int* outUserID);

        Result GetUser(int userID, Models::User* outUser);


        Result PatchUser(int userID, const Models::UserPatch& patch);

    private:
        std::shared_ptr<ConnectionPool> m_connectionPool;
    };
}