#include "gecko/db/FriendshipsTable.h"
#include <utility>

#define DATE_FORMAT "'%m/%d/%Y'"

namespace
{
    // Make userID1 always lt userID2
    // Returns false if they are equal, otherwise true
    bool CanonicalizeFriendshipPair(int* userID1, int* userID2)
    {
        if (*userID1 == *userID2)
            return false;

        if (*userID2 < *userID1)
            std::swap(*userID1, *userID2);

        return true;
    }
}

namespace Gecko::API::DB
{
    FriendshipsTable::Result
    FriendshipsTable::GetActiveFriendships(int userID,
                                           std::vector<std::pair<Models::User, Models::FriendshipMetadata>>* outFriends)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql(
                        "SELECT f.user_2, u.username, DATE_FORMAT(accepted_on, " DATE_FORMAT ") "
                        "FROM Friendships f "
                            "JOIN Users u ON u.user_id=f.user_2 "
                        "WHERE f.user_1=? AND f.friendship_status='accepted' "
                        "UNION ALL "
                        "SELECT f.user_1, u.username, DATE_FORMAT(accepted_on, " DATE_FORMAT ") "
                        "FROM Friendships f "
                            "JOIN Users u ON u.user_id=f.user_1 "
                        "WHERE f.user_2=? AND f.friendship_status='accepted'")
                    .bind(userID, userID)
                    .execute();

            *outFriends = {};
            outFriends->reserve(result.count());

            for (auto row : result)
                outFriends->push_back({
                    Models::User {
                        .userID   = row.get(0).get<int>(),
                        .username = row.get(1).get<std::string>()
                    },
                    Models::FriendshipMetadata {
                        .acceptedOn = row.get(2).get<std::string>()
                    }
                });

            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::GetPendingFriendships(int userID,
                                            std::vector<Models::User>* outIncoming,
                                            std::vector<Models::User>* outOutgoing)
    {
        try
        {
            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql(
                        "SELECT f.user_2, u.username, f.initiated_by "
                        "FROM Friendships f "
                            "JOIN Users u ON u.user_id=f.user_2 "
                        "WHERE f.user_1=? AND f.friendship_status='pending' "
                        "UNION ALL "
                        "SELECT f.user_1, u.username, f.initiated_by "
                        "FROM Friendships f "
                            "JOIN Users u ON u.user_id=f.user_1 "
                        "WHERE f.user_2=? AND f.friendship_status='pending'")
                    .bind(userID, userID)
                    .execute();

            *outIncoming = {};
            *outOutgoing = {};
            outIncoming->reserve(result.count());
            outOutgoing->reserve(result.count());

            for (auto row : result)
            {
                Models::User other
                {
                    .userID   = row.get(0).get<int>(),
                    .username = row.get(1).get<std::string>()
                };

                const int initiatedBy = row.get(2).get<int>();

                if (initiatedBy == userID)
                    outOutgoing->push_back(std::move(other));
                else
                    outIncoming->push_back(std::move(other));
            }

            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::CreatePendingFriendship(int initiatorUserID,
                                              int otherUserID)
    {
        try
        {
            int userID1 = initiatorUserID;
            int userID2 = otherUserID;

            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            const auto result =
                connection->sql(
                        "INSERT INTO Friendships (user_1, user_2, initiated_by, friendship_status) "
                        "VALUES (?, ?, ?, 'pending')")
                    .bind(userID1, userID2, initiatorUserID)
                    .execute();

            return result.getAffectedItemsCount() == 1
                ? Result::Success
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::SetPendingFriendshipActive(int userID1,
                                                 int userID2)
    {
        try
        {
            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            const auto result =
                connection->sql(
                        "UPDATE Friendships "
                        "SET friendship_status='accepted' "
                        "WHERE user_1=? AND user_2=? AND friendship_status='pending'")
                    .bind(userID1, userID2)
                    .execute();

            return result.getAffectedItemsCount() == 1
                ? Result::Success
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::SetPendingFriendshipActive(int userID1,
                                                 int userID2,
                                                 int initiatorUserID)
    {
        try
        {
            if (initiatorUserID != userID1 && initiatorUserID != userID2)
                return Result::Failure;

            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            const auto result =
                connection->sql(
                        "UPDATE Friendships "
                        "SET friendship_status='accepted' "
                        "WHERE user_1=? AND user_2=? AND initiated_by=? AND friendship_status='pending'")
                    .bind(userID1, userID2, initiatorUserID)
                    .execute();

            return result.getAffectedItemsCount() == 1
                ? Result::Success
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::DeleteFriendship(int userID1,
                                       int userID2)
    {
        try
        {
            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            const auto result =
                connection->sql(
                        "DELETE FROM Friendships "
                        "WHERE user_1=? AND user_2=?")
                    .bind(userID1, userID2)
                    .execute();

            return result.getAffectedItemsCount() == 1
                ? Result::Success
                : Result::Failure;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::ActiveFriendshipExists(int userID1,
                                             int userID2,
                                             bool* outExists)
    {
        try
        {
            // note(ben): Returning Failure if userID1 == userID2,
            // because users can never be friends with themselves.
            // In other words: why would you even ask?

            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql(
                        "SELECT 1 FROM Friendships "
                        "WHERE user_1=? AND user_2=? AND friendship_status='accepted' "
                        "LIMIT 1")
                    .bind(userID1, userID2)
                    .execute();

            *outExists = result.hasData() && result.count() == 1;
            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }

    FriendshipsTable::Result
    FriendshipsTable::PendingFriendshipExists(int userID1,
                                              int userID2,
                                              bool* outExists,
                                              int *outInitiatorUserID)
    {
        try
        {
            if (!CanonicalizeFriendshipPair(&userID1, &userID2))
                return Result::Failure;

            auto connection = m_connectionPool->Acquire();

            auto result =
                connection->sql(
                        "SELECT initiated_by FROM Friendships "
                        "WHERE user_1=? AND user_2=? AND friendship_status='pending' "
                        "LIMIT 1")
                    .bind(userID1, userID2)
                    .execute();

            *outExists = result.hasData() && result.count() == 1;

            *outInitiatorUserID = *outExists
                ? result.fetchOne().get(0).get<int>()
                : -1;

            return Result::Success;
        }
        catch (mysqlx::Error&)
        {
            return Result::Failure;
        }
    }
}