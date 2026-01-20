import { ref } from 'vue';
import { Cookies } from '@/core/storage/Cookies.js';
import { Dispatch } from '../dispatch/Dispatch.js';
import { ResourceLockedError, NetworkError, HttpError } from './Errors.js';
import { ResourceMutex } from './Mutex.js';
import { User } from '@/core/models/User.js';

// Stores data needed to make calls over the wire, as well
// as the current active user
class SessionStore {
    constructor(activeUser = null, xsrfCookie = null) {
        this._activeUser = ref(activeUser);
        this._xsrfCookie = xsrfCookie;

        this._state = ref('uninitialized');
        this._mutex = new ResourceMutex();
    }

    // FSM representing the current load state of the
    // session store
    // 'uninitialized' | 'error' | 'loading' | 'loggedout' | 'ready'
    state()        { return this._state; }

    // Returns null if not logged in
    activeUser()   { return this._activeUser; }
    xsrfCookie()   { return this._xsrfCookie; }
    activeUserID() { return this._activeUser.value.userID; }

    // Assigns objects from props to the activeUser object
    // If props is falsy, the activeUser is set to null (logged out)
    updateActiveUserInCache(props) {
        if (!props) {
            this._activeUser.value = null;
        } else {
            this._activeUser.value = this._activeUser.value ?? {};
            Object.assign(this._activeUser.value, props);
        }
    }

    // Returns true if user is logged in
    // Returns false if user is not logged in
    // Throws on any other failure
    async init() {
        this._state.value = 'loading';

        try {
            const [_, isLoggedIn] = await Promise.all([this._initXSRF(), this._initUser()]);
            this._state.value = isLoggedIn ? 'ready' : 'loggedout';
            return isLoggedIn;
        } catch (e) {
            this._state.value = 'error';
            throw e;
        }
    }

    // Sends a message to the server requesting a username change
    async changeUsername(newUsername) {
        if (newUsername === this._activeUser.value['username'])
            return;

        if (!this._mutex.tryLock())
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Patch_ChangeUsername(this.activeUserID(), this.xsrfCookie(), newUsername)
                    .onSuccess(() => resolve())
                    .onNetworkError(() => reject(new NetworkError()))
                    .onHttpError((body, status) => reject(new HttpError(status, body)));
            });

            this.updateActiveUserInCache({
                'username': newUsername
            });
        } finally {
            this._mutex.unlock();
        }
    }

    // Returns true if the user was not already logged out
    // Returns false if the user was already logged out
    // Throws on any other error
    async logOut() {
        if (!this.activeUser())
            return false;

        await new Promise((resolve, reject) => {
            Dispatch.Post_LogOut(this.xsrfCookie())
                .onSuccess(() => resolve())
                .onHttpError((body, status) => reject(new HttpError(status, body)))
                .onNetworkError(() => reject(new NetworkError()));
        });

        this._state.value      = 'loggedout'
        this._activeUser.value = null;
        return true;
    }

    async _initXSRF() {
        const getXSRFCookie = () => Cookies.byName('__Host-xsrf_token');

        this._xsrfCookie = getXSRFCookie();

        if (!this._xsrfCookie) {
            await new Promise((resolve, reject) => {
                Dispatch.Get_XSRF()
                    .onSuccess(()            => resolve())
                    .onHttpError((_, status) => reject(new HttpError(status)))
                    .onNetworkError(()       => reject(new NetworkError()));
            });

            this._xsrfCookie = getXSRFCookie();

            if (!this._xsrfCookie)
                throw new Error(`[SessionStore]: Server didn't set the XSRF cookie`);
        }
    }

    async _initUser() {
        if (!this._mutex.tryLock())
            throw new ResourceLockedError();

        try {
            const responseBody = await new Promise((resolve, reject) => {
                Dispatch.Get_UsersMe()
                    .onSuccess(body => resolve(body))
                    .onHttpError(() => resolve(null))
                    .onNetworkError(() => reject(new NetworkError()));
            });

            if (responseBody) {
                this._activeUser.value = User.fromJSON(responseBody['user']);
                return true;
            }
        } finally {
            this._mutex.unlock();
        }

        return false;
    }
};

export { SessionStore };
