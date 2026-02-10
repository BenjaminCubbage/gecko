import {
    reactive,
    readonly,
    ref
} from 'vue';

import {
    HttpError,
    NetworkError,
    ResourceLockedError
} from '@/core/errors/errors.js';

import { Dispatch }           from '@/core/dispatch/Dispatch.js';
import { equalsIgnoreCase }   from '@/core/string/equalsIgnoreCase.js';
import { Friend }             from '@//models/friend.js';
import { MultiResourceMutex } from '@/core/async/mutex.js';

/*
    Stores the friends associated with a session's active user.
    Contains three collections:
         - activeFriends(),
         - pendingIncoming(), and
         - pendingOutgoing()
*/
export class FriendsStore {
    #active;
    #pendingIn;
    #pendingOut;

    #state;
    #mutex;

    #activeRO;
    #pendingInRO;
    #pendingOutRO;
    #stateRO;

    constructor() {
        this.#active     = reactive([]);
        this.#pendingIn  = reactive([]);
        this.#pendingOut = reactive([]);

        this.#mutex = new MultiResourceMutex();
        this.#state = ref('uninitialized');

        this.#activeRO     = readonly(this.#active);
        this.#pendingInRO  = readonly(this.#pendingIn);
        this.#pendingOutRO = readonly(this.#pendingOut);
        this.#stateRO      = readonly(this.#state);
    }

    get activeFriends() {
        return this.#activeRO;
    }

    get pendingIncoming() {
        return this.#pendingInRO;
    }

    get pendingOutgoing() {
        return this.#pendingOutRO;
    }

    get state() {
        return this.#stateRO;
    }

    /*
        Assign props to friend with userID, if it exists
        Returns the updated friend if it was found, otherwise null
    */
    updateFriendInCacheIfExists(userID, props) {
        const [friend, _] = this.#getFriendInCacheByUserID(userID);

        if (!friend)
            return null;

        Object.assign(friend, props);
        return friend;
    }

    /*
        Sync up with server. If the user is not logged in, set all
        collections to empty.
    */
    async requestResync(session) {
        if (!this.#mutex.tryLockAll())
            throw new ResourceLockedError();

        if (!session?.activeUser.value) {
            FriendsStore.#setArr(this.#active,     []);
            FriendsStore.#setArr(this.#pendingIn,  []);
            FriendsStore.#setArr(this.#pendingOut, []);
            this.#state.value = 'loggedout';
            this.#mutex.unlockAll();
            return;
        }

        this.#state.value = 'loading';

        try {
            const [activeFriends, pendingFriends] = await Promise.all([
                new Promise((resolve, reject) => {
                    Dispatch.Get_Friends(session.activeUserID, session.xsrfCookie)
                        .onSuccess     (body           => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                }),
                new Promise((resolve, reject) => {
                    Dispatch.Get_FriendRequests(session.activeUserID, session.xsrfCookie)
                        .onSuccess     (body           => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                })
            ]);

            FriendsStore.#setArr(this.#active,     activeFriends['friends'].map(Friend.fromJSON));
            FriendsStore.#setArr(this.#pendingIn,  pendingFriends['friend_requests']['incoming'].map(Friend.fromJSON));
            FriendsStore.#setArr(this.#pendingOut, pendingFriends['friend_requests']['outgoing'].map(Friend.fromJSON));
            this.#state.value = 'ready';
        } catch (e) {
            this.#state.value = 'error';
            throw e;
        } finally {
            this.#mutex.unlockAll();
        }
    }

    /* 
        Asks server to send a friend request to the target user.
    */
    async publishCreateFriendRequest(session, targetUser) {
        if (!session?.activeUser.value)
            throw new Error('[FriendsStore]: User not logged in');

        if (targetUser == null)
            throw new Error('[FriendsStore]: User was null or undefined');

        if (session.activeUser.value.userID === targetUser.userID)
            throw new Error(`[FriendsStore]: Can't friend self`);

        if (this.#getFriendInCacheByUserID(targetUser.userID)[0])
            throw new Error('[FriendsStore]: Friendship or friend request with that userID already exists in cache');

        if (!this.#mutex.tryLock(targetUser.userID))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestCreate(session.activeUserID, session.xsrfCookie, targetUser.userID)
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            FriendsStore.#unshiftToArr(this.#pendingOut, Friend.fromOptimistic(targetUser, new Date()));
        } finally {
            this.#mutex.unlock(targetUser.userID);
        }
    }

    /*
        Asks server to accept an incoming friend request.
    */
    async publishAcceptFriendRequest(session, userID) {
        if (!session?.activeUser.value)
            throw new Error('[FriendsStore]: User not logged in');

        const [friend, friendType] = this.#getFriendInCacheByUserID(userID);

        if (!friend || friendType != 'pendingin')
            throw new Error(`[FriendsStore]: User ${userID} has no associate incoming friend request in cache`);

        if (!this.#mutex.tryLock(friend.user.userID))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestAccept(session.activeUserID, session.xsrfCookie, friend.user.userID)
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            friend.setAcceptedOnNow();
            FriendsStore.#removeFromArr(this.#pendingIn, friend);
            FriendsStore.#pushToArr(this.#active, friend);
        } finally {
            this.#mutex.unlock(friend.user.userID);
        }
    }

    /*
        Asks server to delete an existing or pending friendship.
    */
    async publishDeleteFriendOrRequest(session, userID) {
        const [friend, friendType] = this.#getFriendInCacheByUserID(userID);

        if (!friend)
            throw new Error(`[FriendsStore]: User ${userID} is not a pending or active friend`);

        if (!this.#mutex.tryLock(userID))
            throw new ResourceLockedError();

        try {
            if (friendType === 'active')
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_Friend(
                        session.activeUserID,
                        session.xsrfCookie,
                        userID)
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });
            else
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_FriendRequest(
                        session.activeUserID,
                        session.xsrfCookie,
                        userID)
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });

            switch (friendType) {
            case 'active':     FriendsStore.#removeFromArr(this.#active,     friend); break;
            case 'pendingin':  FriendsStore.#removeFromArr(this.#pendingIn,  friend); break;
            case 'pendingout': FriendsStore.#removeFromArr(this.#pendingOut, friend); break;
            }
        } finally {
            this.#mutex.unlock(userID);
        }
    }

    /*
        Returns the first friendship matching the predicate,
        as well as the type of that friendship, as a pair.
        Returns [null, null] if no friend was found.
        Return value is like:
        [
            [0]: readonly({ user_id: <userid>, ...}) | null,
            [1]: 'active' | 'pendingin' | 'pendingout' | null
        ]
    */
    getFriendInCacheByPredicate(predicate) {
        return FriendsStore.#findInArr(this.activeFriends,   'active',     predicate) ??
               FriendsStore.#findInArr(this.pendingIncoming, 'pendingin',  predicate) ??
               FriendsStore.#findInArr(this.pendingOutgoing, 'pendingout', predicate) ??
               [null, null];
    }

    /*
        Ditto
    */
    getFriendInCacheByUsername(username) {
        return this.getFriendInCacheByPredicate(
            friend => equalsIgnoreCase(friend.user.username, username));
    }

    /*
        Private variants: get non-readonly friends by predicate
    */

    #getFriendInCacheByPredicate(predicate) {
        return FriendsStore.#findInArr(this.#active,     'active',     predicate) ??
               FriendsStore.#findInArr(this.#pendingIn,  'pendingin',  predicate) ??
               FriendsStore.#findInArr(this.#pendingOut, 'pendingout', predicate) ??
               [null, null];
    }

    #getFriendInCacheByUserID(userID) {
        return this.#getFriendInCacheByPredicate(
            friend => friend.user.userID === userID);
    }

    static #setArr(arr, newValue) {
        arr.splice(0, arr.length);
        arr.push(...newValue);
    }

    static #pushToArr(arr, friend) {
        arr.push(friend);
    }

    static #unshiftToArr(arr, friend) {
        arr.unshift(friend);
    }

    static #removeFromArr(arr, friend) {
        const index = arr.indexOf(friend);
        if (index === -1)
            return false;
        arr.splice(index, 1);
        return true;
    }

    /*
        Returns null (not a pair) on failure for convenient
        chaining.
    */
    static #findInArr(arr, type, predicate) {
        const found = arr.find(predicate);
        if (found)
            return [found, type];
        return null;
    }
}