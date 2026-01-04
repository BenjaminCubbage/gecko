#include "gecko/services/FriendshipsService.h"

#define EXPECT(cond, failResult) do { if (!(cond)) return failResult; } while(0)

namespace Gecko::API::Services
{
    FriendshipsService::Result
    FriendshipsService::GetFriendships(int userID,
                                       std::vector<std::pair<Models::User, Models::FriendshipMetadata>>* outFriends)
    {
        if (m_dbFriendships->GetActiveFriendships(userID, outFriends) == DB::FriendshipsTable::Result::Success)
        {
            if (outFriends->size() > 0)
                return Result::Success;

            // note(ben): Distinguish "no friends" from "no such user"
            bool exists{};
            EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::Success,
                   Result::DatabaseError);
            EXPECT(exists, Result::UserNotFound);

            return Result::Success;
        }

        return Result::DatabaseError;
    }

    FriendshipsService::Result
    FriendshipsService::FriendshipExists(int userID1,
                                         int userID2,
                                         bool* outExists)
    {
        EXPECT(userID1 != userID2, Result::SelfFriendNotAllowed);

        EXPECT(m_dbFriendships->ActiveFriendshipExists(userID1, userID2, outExists)
               == DB::FriendshipsTable::Result::Success,
               Result::DatabaseError);

        if (*outExists)
            return Result::Success;

        // note(ben): Distinguish "not friends" from "user(s) don't exist"
        bool user1Exists{};
        EXPECT(m_dbUsers->UserExists(userID1, &user1Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user1Exists, Result::User1NotFound);

        bool user2Exists{};
        EXPECT(m_dbUsers->UserExists(userID2, &user2Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user2Exists, Result::User2NotFound);

        return Result::Success;
    }

    FriendshipsService::Result
    FriendshipsService::GetFriendRequests(int userID,
                                          std::vector<Models::User>* outIncoming,
                                          std::vector<Models::User>* outOutgoing)
    {
        if (m_dbFriendships->GetPendingFriendships(userID, outIncoming, outOutgoing) == DB::FriendshipsTable::Result::Success)
        {
            if (outIncoming->size() > 0 || outOutgoing->size() > 0)
                return Result::Success;

            // note(ben): Distinguish "no requests" from "no such user"
            bool exists{};
            EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::Success,
                   Result::DatabaseError);
            EXPECT(exists, Result::UserNotFound);

            return Result::Success;
        }

        bool exists{};
        EXPECT(m_dbUsers->UserExists(userID, &exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(exists, Result::UserNotFound);

        return Result::DatabaseError;
    }

    FriendshipsService::Result
    FriendshipsService::CreateFriendRequest(int initiatorUserID,
                                            int otherUserID)
    {
        EXPECT(initiatorUserID != otherUserID, Result::SelfFriendNotAllowed);

        if (m_dbFriendships->CreatePendingFriendship(initiatorUserID, otherUserID) == DB::FriendshipsTable::Result::Success)
            return Result::Success;

        bool activeExists{};
        EXPECT(m_dbFriendships->ActiveFriendshipExists(initiatorUserID, otherUserID, &activeExists)
               == DB::FriendshipsTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!activeExists, Result::AlreadyFriends);

        bool pendingExists{};
        int pendingInitiator = -1;
        EXPECT(m_dbFriendships->PendingFriendshipExists(initiatorUserID, otherUserID, &pendingExists, &pendingInitiator)
               == DB::FriendshipsTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!pendingExists, Result::FriendRequestAlreadyExists);

        bool user1Exists{};
        EXPECT(m_dbUsers->UserExists(initiatorUserID, &user1Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user1Exists, Result::User1NotFound);

        bool user2Exists{};
        EXPECT(m_dbUsers->UserExists(otherUserID, &user2Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user2Exists, Result::User2NotFound);

        return Result::DatabaseError;
    }

    FriendshipsService::Result
    FriendshipsService::AcceptFriendRequest(int accepterUserID,
                                            int otherUserID)
    {
        EXPECT(accepterUserID != otherUserID, Result::SelfFriendNotAllowed);

        if (m_dbFriendships->SetPendingFriendshipActive(accepterUserID,
                                                        otherUserID,
                                                        otherUserID)
            == DB::FriendshipsTable::Result::Success)
        {
            return Result::Success;
        }

        bool activeExists{};
        EXPECT(m_dbFriendships->ActiveFriendshipExists(accepterUserID, otherUserID, &activeExists)
               == DB::FriendshipsTable::Result::Success,
               Result::DatabaseError);
        EXPECT(!activeExists, Result::AlreadyFriends);

        bool user1Exists{};
        EXPECT(m_dbUsers->UserExists(accepterUserID, &user1Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user1Exists, Result::User1NotFound);

        bool user2Exists{};
        EXPECT(m_dbUsers->UserExists(otherUserID, &user2Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user2Exists, Result::User2NotFound);

        return Result::FriendRequestNotFound;
    }

    FriendshipsService::Result
    FriendshipsService::DeleteFriendshipOrFriendRequest(int userID1,
                                                        int userID2)
    {
        EXPECT(userID1 != userID2, Result::SelfFriendNotAllowed);

        if (m_dbFriendships->DeleteFriendship(userID1, userID2) == DB::FriendshipsTable::Result::Success)
            return Result::Success;

        // note(ben): Distinguish "not friends / no request" from "user(s) don't exist"
        bool user1Exists{};
        EXPECT(m_dbUsers->UserExists(userID1, &user1Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user1Exists, Result::User1NotFound);

        bool user2Exists{};
        EXPECT(m_dbUsers->UserExists(userID2, &user2Exists) == DB::UsersTable::Result::Success,
               Result::DatabaseError);
        EXPECT(user2Exists, Result::User2NotFound);

        // note(ben): If a relationship still exists, the delete failing is a DB error.
        bool activeExists{};
        EXPECT(m_dbFriendships->ActiveFriendshipExists(userID1, userID2, &activeExists)
               == DB::FriendshipsTable::Result::Success,
            Result::DatabaseError);
        EXPECT(!activeExists, Result::DatabaseError);

        bool pendingExists{};
        int pendingInitiator = -1;
        EXPECT(m_dbFriendships->PendingFriendshipExists(userID1,
                                                        userID2,
                                                        &pendingExists,
                                                        &pendingInitiator)
               == DB::FriendshipsTable::Result::Success,
               Result::DatabaseError);

        return pendingExists
            ? Result::DatabaseError
            : Result::NotFriends;
    }
}