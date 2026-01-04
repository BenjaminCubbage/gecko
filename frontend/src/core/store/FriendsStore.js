import { ref, reactive, readonly } from 'vue';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { equalsIgnoreCase } from '@/core/string/EqualsIgnoreCase.js';
import { Friend } from '../models/Friend.js';
import { MultiResourceMutex } from './Mutex.js';
import { NetworkError, HttpError, ResourceLockedError } from './Errors.js';

// Stores the friends associated with a session's active user.
// Contains three collections:
//      - activeFriends(),
//      - pendingIncoming(), and
//      - pendingOutgoing()
// Modifications to these collections should only be performed
// via functions on the FriendsStore object as the underlying
// Vue refs are readonly
class FriendsStore {
    constructor() {
        this._active     = reactive([]);
        this._pendingIn  = reactive([]);
        this._pendingOut = reactive([]);

        this._state = ref('uninitialized');
        this._mutex = new MultiResourceMutex();

        this._activeRO     = readonly(this._active);
        this._pendingInRO  = readonly(this._pendingIn);
        this._pendingOutRO = readonly(this._pendingOut);
        this._stateRO      = readonly(this._state);
    }

    // Current FSM state of the store
    // 'uninitialized' | 'loggedout' | 'error' | 'loading' | 'ready'
    state() {
        return this._stateRO;
    }

    activeFriends()   { return this._activeRO;     }
    pendingIncoming() { return this._pendingInRO;  }
    pendingOutgoing() { return this._pendingOutRO; }

    // Assign props to friend with userID, if it exists
    // Returns the updated friend if it was found, otherwise null
    updateFriendInCacheIfExists(userID, props) {
        const [friend, _] = this._getRawFriendInCacheByUserID(userID);

        if (!friend)
            return null;

        Object.assign(friend, props);
        return friend;
    }

    // Sync up with server
    // If the user is not logged in, set all collections to
    // empty and set state to 'loggedout'
    async resync(session) {
        if (!session?.activeUser().value) {
            this._setArr(this._active,     []);
            this._setArr(this._pendingIn,  []);
            this._setArr(this._pendingOut, []);
            this._state.value = 'loggedout';
            return;
        }

        if (!this._mutex.tryLockAll())
            throw new ResourceLockedError();

        this._state.value = 'loading';

        try {
            const [activeFriends, pendingFriends] = await Promise.all([
                new Promise((resolve, reject) => {
                    Dispatch.Get_Friends(session.activeUserID(), session.xsrfCookie())
                        .onSuccess     (body           => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                }),
                new Promise((resolve, reject) => {
                    Dispatch.Get_FriendRequests(session.activeUserID(), session.xsrfCookie())
                        .onSuccess     (body           => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                })
            ]);

            this._setArr(this._active,     activeFriends['friends'].map(Friend.fromJSON));
            this._setArr(this._pendingIn,  pendingFriends['friend_requests']['incoming'].map(Friend.fromJSON));
            this._setArr(this._pendingOut, pendingFriends['friend_requests']['outgoing'].map(Friend.fromJSON));
            this._state.value = 'ready';
        } catch (e) {
            this._state.value = 'error';
            throw e;
        } finally {
            this._mutex.unlockAll();
        }
    }

    async init(session) {
        await this.resync(session);
    }

    // Send a friend request to another user
    async createFriendRequest(session, targetUser) {
        if (!session?.activeUser().value)
            throw new Error('[FriendsStore]: User not logged in');

        if (targetUser == null)
            throw new Error('[FriendsStore]: User was null or undefined');

        if (session.activeUser().value.userID === targetUser.userID)
            throw new Error(`[FriendsStore]: Can't friend self`);

        if (this._getRawFriendInCacheByUserID(targetUser.userID)[0])
            throw new Error('[FriendsStore]: Friendship or friend request with that userID already exists in cache');

        if (!this._mutex.tryLock(targetUser.userID))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestCreate(session.activeUserID(), session.xsrfCookie(), targetUser.userID)
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            this._unshiftToArr(this._pendingOut, Friend.fromOptimistic(targetUser, new Date()));
        } finally {
            this._mutex.unlock(targetUser.userID);
        }
    }

    // Accept an incoming pending friendship
    async acceptFriendRequest(session, userID) {
        if (!session?.activeUser().value)
            throw new Error('[FriendsStore]: User not logged in');
        
        const [friend, friendType] = this._getRawFriendInCacheByUserID(userID);
        
        if (!friend || friendType != 'pendingin')
            throw new Error(`[FriendsStore]: User ${userID} has no associate incoming friend request in cache`);

        if (!this._mutex.tryLock(friend.user.userID))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestAccept(session.activeUserID(), session.xsrfCookie(), friend.user.userID)
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            friend.setAcceptedOnNow();
            this._removeFromArr(this._pendingIn, friend);
            this._pushToArr(this._active, friend);
        } finally {
            this._mutex.unlock(friend.user.userID);
        }
    }

    // Delete an active or pending friendship by the other user's ID
    async deleteFriendOrRequest(session, userID) {
        const [friend, friendType] = this._getRawFriendInCacheByUserID(userID);

        if (!friend)
            throw new Error(`[FriendsStore]: User ${userID} is not a pending or active friend`);

        if (!this._mutex.tryLock(userID))
            throw new ResourceLockedError();

        try {
            if (friendType === 'active')
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_Friend(session.activeUserID(),
                                           session.xsrfCookie(),
                                           userID)
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });
            else
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_FriendRequest(session.activeUserID(),
                                                  session.xsrfCookie(),
                                                  userID)
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });

            switch (friendType) {
                case 'active':     this._removeFromArr(this._active,     friend); break;
                case 'pendingin':  this._removeFromArr(this._pendingIn,  friend); break;
                case 'pendingout': this._removeFromArr(this._pendingOut, friend); break;
            }
        } finally {
            this._mutex.unlock(userID);
        }
    }

    // Returns the first friendship matching the predicate,
    // as well as the type of that friendship, as a pair.
    // Returns [null, null] if no friend was found.
    // Return value is like:
    // [
    //     [0]: readonly({ user_id: <userid>, ...}) | null,
    //     [1]: 'active' | 'pendingin' | 'pendingout' | null
    // ]
    getFriendInCacheByPredicate(predicate) {
        return this._findInArr(this.activeFriends(),   'active',     predicate) ??
               this._findInArr(this.pendingIncoming(), 'pendingin',  predicate) ??
               this._findInArr(this.pendingOutgoing(), 'pendingout', predicate) ??
               [null, null];
    }

    getFriendInCacheByUsername(username) {
        return this.getFriendInCacheByPredicate(
            friend => equalsIgnoreCase(friend.user.username, username));
    }

    _getRawFriendInCacheByPredicate(predicate) {
        return this._findInArr(this._active,     'active',     predicate) ??
               this._findInArr(this._pendingIn,  'pendingin',  predicate) ??
               this._findInArr(this._pendingOut, 'pendingout', predicate) ??
               [null, null];
    }

    _getRawFriendInCacheByUserID(userID) {
        return this._getRawFriendInCacheByPredicate(
            friend => friend.user.userID === userID);
    }

    _setArr(arr, newValue) {
        arr.splice(0, arr.length);
        arr.push(...newValue);
    }

    _pushToArr(arr, friend) {
        arr.push(friend);
    }

    _unshiftToArr(arr, friend) {
        arr.unshift(friend);
    }

    _removeFromArr(arr, friend) {
        let index = arr.indexOf(friend);
        if (index === -1)
            return false;
        arr.splice(index, 1);
        return true;
    }

    // Note that this function returns null on failure,
    // not a pair of nulls. 

    // This signature is designed for convenient chaining 
    // (see getFriendInCache*)
    _findInArr(arr, type, predicate) {
        const found = arr.find(predicate);
        if (found)
            return [found, type];

        return null;
    }
}

export { FriendsStore };