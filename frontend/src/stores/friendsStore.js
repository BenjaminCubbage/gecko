import {
    computed,
    reactive,
    readonly,
    ref,
    toRaw
} from 'vue';

import {
    HttpError,
    NetworkError,
    ResourceLockedError
} from '@/core/errors/errors.js';

import { Dispatch }           from '@/core/dispatch/Dispatch.js';
import { equalsIgnoreCase }   from '@/core/string/equalsIgnoreCase.js';
import { MultiResourceMutex } from '@/core/async/mutex.js';

import { 
    FriendStatus, 
    Friend 
} from '@/models/friend.js';

/*
    Stores the friends associated with a session's active user.

    Contains one main collection, allFriends, which includes
    pending incoming and outgoing friendships, as well as three
    derived views:
         - pendingIncoming
         - pendingOutgoing
         - activeFriends

    They are stored in that order in the allFriends getter.
*/
export class FriendsStore {
    #friends;

    #active;
    #pendingIn;
    #pendingOut;

    #state;
    #mutex;

    #friendsRO;
    #stateRO;

    constructor() {
        this.#friends = reactive([]);

        this.#mutex = new MultiResourceMutex();
        this.#state = ref('uninitialized');

        this.#active     = computed(() => this.#friends.filter(f => f.status === FriendStatus.Active));
        this.#pendingIn  = computed(() => this.#friends.filter(f => f.status === FriendStatus.PendingIncoming));
        this.#pendingOut = computed(() => this.#friends.filter(f => f.status === FriendStatus.PendingOutgoing));

        this.#friendsRO = readonly(this.#friends);
        this.#stateRO   = readonly(this.#state);
    }

    get allFriends() {
        return this.#friendsRO;
    }

    get activeFriends() {
        return this.#active;
    }

    get pendingIncoming() {
        return this.#pendingIn;
    }

    get pendingOutgoing() {
        return this.#pendingOut;
    }

    get state() {
        return this.#stateRO;
    }

    /*
        Returns the friend with the provided user ID, if it exists.
    */
    getFriendByUserID(userID) {
        return this.#friends.find(f => f.user.userID === userID);
    }

    getFriendByUsername(username) {
        return this.#friends.find(f => f.user.username.toLowerCase() === username.toLowerCase());
    }

    /*
        Assign props to friend with userID, if it exists
        Returns the updated friend if it was found, otherwise null
    */
    updateFriendInCacheIfExists(userID, props) {
        const friend = this.getFriendByUserID(userID);

        if (!friend)
            return null;

        Object.assign(friend, props);
        return friend;
    }

    /*
        Sync up with server. If the user is not logged in, set friends
        collection to empty.
    */
    async requestResync(session) {
        if (!this.#mutex.tryLockAll())
            throw new ResourceLockedError();

        if (!session?.activeUser.value) {
            FriendsStore.#setArr(this.#friends, []);
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

            FriendsStore.#setArr(this.#friends,
                [
                    ...pendingFriends['friend_requests']['incoming'].map(json => Friend.fromJSON(json, FriendStatus.PendingIncoming)),
                    ...pendingFriends['friend_requests']['outgoing'].map(json => Friend.fromJSON(json, FriendStatus.PendingOutgoing)),
                    ...activeFriends['friends'].map(json => Friend.fromJSON(json, FriendStatus.Active))
                ]
            );

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
        targetUser = toRaw(targetUser);

        if (import.meta.env.DEV) {
            if (!session?.activeUser.value)
                throw new Error('[FriendsStore]: User not logged in');

            if (targetUser == null)
                throw new Error('[FriendsStore]: User was null or undefined');

            if (session.activeUser.value.userID === targetUser.userID)
                throw new Error(`[FriendsStore]: Can't friend self`);

            if (this.#friends.some(f => f.user.userID === targetUser.userID))
                throw new Error('[FriendsStore]: Friendship or request with that userID already exists in cache');
        }

        if (!this.#mutex.tryLock(targetUser.userID))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestCreate(session.activeUserID, session.xsrfCookie, targetUser.userID)
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            FriendsStore.#unshiftToArr(
                this.#friends, 
                Friend.fromUser(targetUser, FriendStatus.PendingOutgoing));
        } finally {
            this.#mutex.unlock(targetUser.userID);
        }
    }

    /*
        Asks server to accept an incoming friend request.
    */
    async publishAcceptFriendRequest(session, userID) {
        const friend = this.getFriendByUserID(userID);

        if (import.meta.env.DEV) {
            if (!session?.activeUser.value)
                throw new Error('[FriendsStore]: User not logged in');

            if (friend == null || friend.status !== FriendStatus.PendingIncoming)
                throw new Error(`[FriendsStore]: User ID ${userID} has no associate incoming friend request in cache`);
        }

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
        } finally {
            this.#mutex.unlock(friend.user.userID);
        }
    }

    /*
        Asks server to delete an existing or pending friendship.
    */
    async publishDeleteFriendOrRequest(session, userID) {
        const friend = this.getFriendByUserID(userID);

        if (import.meta.env.DEV) {
            if (!friend)
                throw new Error(`[FriendsStore]: User ${userID} is not a pending or active friend`);
        }

        if (!this.#mutex.tryLock(userID))
            throw new ResourceLockedError();

        try {
            if (friend.status === FriendStatus.Active)
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

            FriendsStore.#removeFromArr(this.#friends, friend);
        } finally {
            this.#mutex.unlock(userID);
        }
    }

    static #setArr(arr, newValue) {
        arr.length = 0;
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
}