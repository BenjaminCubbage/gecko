#pragma once
#include <vector>
#include "gecko/db/ConnectionPool.h"
#include "gecko/models/FriendshipMetadata.h"
#include "gecko/models/User.h"
#include "mysqlx/xdevapi.h"

namespace Gecko::API::DB
{
    class FriendshipsTable
    {
      public:
        enum class Result { OK, Failure };

        explicit FriendshipsTable(ConnectionPool* connectionPool)
            : m_connectionPool(connectionPool) {}

        // Accepted friendships involving userID.
        Result GetActiveFriendships(int userID,
                                    std::vector<std::pair<Models::User, Models::FriendshipMetadata>>* outFriends);

        // Pending friendships involving userID, split by direction.
        // Incoming: initiated_by != userID
        // Outgoing: initiated_by == userID
        Result GetPendingFriendships(int userID,
                                     std::vector<Models::User>* outIncoming,
                                     std::vector<Models::User>* outOutgoing);

        // Creates a new pending friendship request initiated by initiatorUserID.
        // Fails if the friendship already exists (pending or accepted) or if user IDs are invalid.
        Result CreatePendingFriendship(int initiatorUserID,
                                       int otherUserID);

        // Accepts a pending friendship (regardless of who initiated it).
        // Must fail if the friendship does not exist or is not currently pending.
        Result SetPendingFriendshipActive(int userID1,
                                          int userID2);

        // Accepts a pending friendship iff the request is initiated by initiatorUserID.
        // Otherwise, fails.
        Result SetPendingFriendshipActive(int userID1,
                                          int userID2,
                                          int initiatorUserID);

        // Deletes any friendship row (pending or accepted).
        // Must fail if the friendship does not exist.
        Result DeleteFriendship(int userID1,
                                int userID2);

        // Checks whether an accepted friendship exists between the two users.
        Result ActiveFriendshipExists(int userID1,
                                      int userID2,
                                      bool* outExists);

        // Checks whether a pending friendship request exists between the two users (either direction).
        Result PendingFriendshipExists(int userID1,
                                       int userID2,
                                       bool* outExists,
                                       int* initiatorUserID);

      private:
        ConnectionPool* m_connectionPool;
    };
}