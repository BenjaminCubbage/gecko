#pragma once
#include <utility>
#include <vector>
#include "gecko/db/FriendshipsTable.h"
#include "gecko/db/UsersTable.h"
#include "gecko/models/User.h"
#include "gecko/models/FriendshipMetadata.h"

namespace Gecko::API::Services
{
    class FriendshipsService
    {
      public:
        enum class Result
        {
            OK,

            /* Service-space */
            SelfFriendNotAllowed,

            /* DB-space */
            AlreadyFriends,
            FriendRequestAlreadyExists,
            FriendRequestNotFound,
            NotFriends,
            UserNotFound,
            User1NotFound,
            User2NotFound,

            /* DB fallthrough */
            DatabaseError
        };

        FriendshipsService(DB::FriendshipsTable* dbFriendships,
                           DB::UsersTable* dbUsers)
            : m_dbFriendships(dbFriendships),
              m_dbUsers(dbUsers) {}

        // Accepted friendships
        Result GetFriendships(int userID,
                              std::vector<std::pair<Models::User, Models::FriendshipMetadata>>* outFriends);

        Result FriendshipExists(int userID1,
                                int userID2,
                                bool* outExists);

        // Pending friendships (friend requests)
        Result GetFriendRequests(int userID,
                                 std::vector<Models::User>* outIncoming,
                                 std::vector<Models::User>* outOutgoing);

        Result CreateFriendRequest(int initiatorUserID,
                                   int otherUserID);

        // Accepts an incoming request: (accepterUserID <-> otherUserID) must exist and be pending.
        Result AcceptFriendRequest(int accepterUserID,
                                   int otherUserID);

        // Deletes either an accepted friendship or a pending request between the two users.
        Result DeleteFriendshipOrFriendRequest(int userID1,
                                               int userID2);

      private:
        DB::FriendshipsTable* m_dbFriendships;
        DB::UsersTable*       m_dbUsers;
    };
}
