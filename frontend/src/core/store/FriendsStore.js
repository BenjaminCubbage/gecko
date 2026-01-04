import { ref, shallowRef, readonly, triggerRef } from 'vue';
import { Dispatch } from '../dispatch/Dispatch.js';
import { equalsIgnoreCase } from '@/core/string/EqualsIgnoreCase.js';
import { NetworkError, HttpError, ResourceLockedError } from './Errors.js';
import { MultiResourceMutex } from './Mutex.js';

class FriendsStore {
    constructor() {
        this._active     = shallowRef([]);
        this._pendingIn  = shallowRef([]);
        this._pendingOut = shallowRef([]);

        this._activeRO     = readonly(this._active);
        this._pendingInRO  = readonly(this._pendingIn);
        this._pendingOutRO = readonly(this._pendingOut);

        this._locks  = new MultiResourceMutex();

        this._state   = ref('uninitialized');
    }

    // Current FSM state of the store
    // 'uninitialized' | 'error' | 'loading' | 'ready'
    state() { 
        return this._state;
    }

    activeFriends()   { return this._activeRO; }
    pendingIncoming() { return this._pendingInRO; }
    pendingOutgoing() { return this._pendingOutRO; }

    // Returns a pair of the friend user object and the friendship 
    // type matching the user id, or a pair of null values if not 
    // found.
    // Return value like:
    // [
    //     [0]: { user_id: <userid>, ...} | null,
    //     [1]: 'active' | 'pendingin' | 'pendingout' | null
    // ]
    getFriendInCacheByUserID(userID) {
        const existingActive 
            = this._active.value.find(u => u['user_id'] == userID);
        if (existingActive)
            return [existingActive, 'active'];

        const existingPendingIn 
            = this._pendingIn.value.find(u => u['user_id'] == userID);
        if (existingPendingIn)
            return [existingPendingIn, 'pendingin'];

        const existingPendingOut 
            = this._pendingOut.value.find(u => u['user_id'] == userID);
        if (existingPendingOut)
            return [existingPendingOut, 'pendingout'];

        return [null, null];
    }

    // Ditto (case-insensitive)
    getFriendInCacheByUsername(username) {
        const existingActive 
            = this._active.value.find(u => equalsIgnoreCase(u.username, username));
        if (existingActive)
            return [existingActive, 'active'];

        const existingPendingIn 
            = this._pendingIn.value.find(u => equalsIgnoreCase(u.username, username));
        if (existingPendingIn)
            return [existingPendingIn, 'pendingin'];

        const existingPendingOut 
            = this._pendingOut.value.find(u => equalsIgnoreCase(u.username, username));
        if (existingPendingOut)
            return [existingPendingOut, 'pendingout'];

        return [null, null];
    }

    // Assign props to friend with userID, if it exists
    // Returns the updated friend if it was found, otherwise null
    updateFriendInCacheIfExists(userID, props) {
        const [friend, friendType] = this.getFriendInCacheByUserID(userID);

        if (!friend)
            return null;

        Object.assign(friend, props);
        switch (friendType) {
            case 'pendingin':  triggerRef(this._pendingIn);  break;
            case 'pendingout': triggerRef(this._pendingOut); break;
            case 'active':     triggerRef(this._active);     break;
        }

        return friend;
    }

    // Sync up with server
    // If the user is not logged in, set all collections to
    // empty and set state to 'error'
    async resync(session) {
        if (!this._locks.tryLockAll())
            throw new ResourceLockedError();

        if (!session?.activeUser().value) {
            this._setArr(this._active,     []);
            this._setArr(this._pendingIn,  []);
            this._setArr(this._pendingOut, []);
            this._state.value = 'error';
            return;
        }

        this._state.value = 'loading';

        try {
            const [activeFriends, pendingFriends] = await Promise.all([
                new Promise((resolve, reject) => {
                    Dispatch.Get_Friends(session.activeUserID(), session.xsrfCookie())
                        .onSuccess     (body => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                }),
                new Promise((resolve, reject) => {
                    Dispatch.Get_FriendRequests(session.activeUserID(), session.xsrfCookie())
                        .onSuccess     (body => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                })
            ]);

            this._setArr(this._active,     activeFriends['friends'].map(v => v['user']));
            this._setArr(this._pendingIn,  pendingFriends['friend_requests']['incoming'].map(v => v['user']));
            this._setArr(this._pendingOut, pendingFriends['friend_requests']['outgoing'].map(v => v['user']));
            this._state.value = 'ready';
        } catch (e) {
            this._state.value = 'error';
            throw e;
        } finally {
            this._locks.unlockAll();
        }
    }

    async init(session) {
        await this.resync(session);
    }

    // Send a friend request to another user
    async createFriendRequest(session, user) {
        if (!session?.activeUser().value)
            throw new Error('[FriendsStore]: User not logged in');

        if (session.activeUser().value['user_id'] == user['user_id'])
            throw new Error(`[FriendsStore]: Can't friend self`);

        if (this._active    .value.some(u => u['user_id'] == user['user_id']) ||
            this._pendingIn .value.some(u => u['user_id'] == user['user_id']) ||
            this._pendingOut.value.some(u => u['user_id'] == user['user_id'])) {
            throw new Error('[FriendsStore]: Friendship or friend request with that userID already exists in cache');
        }

        if (!this._locks.tryLock(user['user_id']))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestCreate(session.activeUserID(), session.xsrfCookie(), user['user_id'])
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError));
            });

            this._unshiftToArr(this._pendingOut, user);
        } finally {
            this._locks.unlock(user['user_id']);
        }
    }

    // Accept an incoming pending friendship
    async acceptFriendRequest(session, friend) {
        if (!session?.activeUser().value)
            throw new Error('[FriendsStore]: User not logged in');

        if (!this._pendingIn.value.includes(friend))
            throw new Error('[FriendsStore]: Friend not incoming (not in cache)');

        if (!this._locks.tryLock(friend['user_id']))
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_FriendRequestAccept(session.activeUserID(), session.xsrfCookie(), friend['user_id'])
                    .onSuccess     (()             => resolve())
                    .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(()             => reject(new NetworkError()));
            });

            this._removeFromArr(this._pendingIn, friend);
            this._pushToArr(this._active, friend);
        } finally {
            this._locks.unlock(friend['user_id']);
        }
    }

    // Delete an active or pending friendship.
    async deleteFriendOrRequest(session, friend) {
        let isPending  = false;

        if (this._pendingIn.value.includes(friend) ||
            this._pendingOut.value.includes(friend)) {
            isPending = true;
        }

        if (!isPending && !this._active.value.includes(friend))
            throw new Error(`[FriendsStore]: User is not a pending or active friend`);

        if (!this._locks.tryLock(friend['user_id']))
            throw new ResourceLockedError();

        try {
            if (isPending)
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_FriendRequest(session.activeUserID(), 
                                                  session.xsrfCookie(), 
                                                  friend['user_id'])
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });
            else
                await new Promise((resolve, reject) => {
                    Dispatch.Delete_Friend(session.activeUserID(), 
                                           session.xsrfCookie(), friend['user_id'])
                        .onSuccess     (()             => resolve())
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                });

            if (isPending) {
                if (!this._removeFromArr(this._pendingIn, friend))
                    this._removeFromArr(this._pendingOut, friend);
            } else
                this._removeFromArr(this._active, friend);
        } finally {
            this._locks.unlock(friend['user_id']);
        }
    }

    _setArr(arr, newValue) {
        arr.value = newValue;
        triggerRef(arr);
    }

    _pushToArr(arr, friend) {
        arr.value.push(friend);
        triggerRef(arr);
    }

    _unshiftToArr(arr, friend) {
        arr.value.unshift(friend);
        triggerRef(arr);
    }

    _removeFromArr(arr, friend) {
        let index = arr.value.indexOf(friend);
        if (index === -1)
            return false;
        arr.value.splice(index, 1);
        triggerRef(arr);
        return true;
    }
}

export { FriendsStore };