import {
    readonly,
    ref
} from 'vue';

import {
    ResourceLockedError,
    NetworkError,
    HttpError
} from '@/core/errors/errors.js';

import { Cookies }       from '@/core/http/cookies.js';
import { Dispatch }      from '@/core/dispatch/Dispatch.js';
import { ResourceMutex } from '@/core/async/mutex.js';
import { User }          from '@//models/user.js';

/*
    Stores the current user and XSRF cookie.
*/
class SessionStore {
    #activeUser;
    #xsrfCookie;

    #mutex;
    #state;

    #activeUserRO;
    #stateRO;

    constructor() {
        this.#activeUser = ref(null);
        this.#xsrfCookie = null;

        this.#mutex = new ResourceMutex();
        this.#state = ref('uninitialized');

        this.#activeUserRO = readonly(this.#activeUser);
        this.#stateRO      = readonly(this.#state);
    }

    get activeUser() {
        return this.#activeUserRO;
    }

    get activeUserID() {
        return this.#activeUser.value.userID;
    }

    get xsrfCookie() {
        return this.#xsrfCookie;
    }

    get state() {
        return this.#stateRO;
    }

    /*
        Sync the active user to the server. Returns whether or not
        the user was logged in.
    */
    async requestResync() {
        this.#state.value = 'loading';

        try {
            const [_, isLoggedIn] = await Promise.all([
                this.#retrieveValidXSRF(),
                this.#retrieveActiveUser()]);

            this.#state.value = isLoggedIn
                ? 'ready'
                : 'loggedout';

            return isLoggedIn;
        } catch (e) {
            this.#state.value      = 'error';
            this.#activeUser.value = null;
            throw e;
        }
    }

    /*
        Sends a request to the server to change the active user's
        username. Patches pessimistically.
    */
    async requestChangeUsername(newUsername) {
        if (newUsername === this.#activeUser.value.username)
            return;

        if (!this.#mutex.tryLock())
            throw new ResourceLockedError();

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Patch_ChangeUsername(this.activeUserID, this.xsrfCookie, newUsername)
                    .onSuccess(() => resolve())
                    .onNetworkError(() => reject(new NetworkError()))
                    .onHttpError((body, status) => reject(new HttpError(status, body)));
            });

            this.#activeUser.value.username = newUsername;
        } finally {
            this.#mutex.unlock();
        }
    }

    /*
        Returns false if the user was already logged out.
    */
    async requestLogOut() {
        if (this.#activeUser.value == null)
            return false;

        try {
            await new Promise((resolve, reject) => {
                Dispatch.Post_LogOut(this.xsrfCookie)
                    .onSuccess(() => resolve())
                    .onHttpError((body, status) => reject(new HttpError(status, body)))
                    .onNetworkError(() => reject(new NetworkError()));
            });
        } catch (e) {
            /*
                If we're unauthorized we're already logged out
            */
            if (e.status !== 401)
                throw e;
        }

        this.#state.value      = 'loggedout'
        this.#activeUser.value = null;
        return true;
    }

    /*
        Gets a valid XSRF token, either from the server or from an
        already-stored cookie.
    */
    async #retrieveValidXSRF() {
        const cookie = () =>
            Cookies.getXSRFToken();

        this.#xsrfCookie = cookie();

        if (!this.#xsrfCookie) {
            await new Promise((resolve, reject) => {
                Dispatch.Get_XSRF()
                    .onSuccess(()            => resolve())
                    .onHttpError((_, status) => reject(new HttpError(status)))
                    .onNetworkError(()       => reject(new NetworkError()));
            });

            this.#xsrfCookie = cookie();

            if (!this.#xsrfCookie)
                throw new Error(`[SessionStore]: Server didn't set the XSRF cookie`);
        }
    }

    /*
        Retrieves and updates the active user.
    */
    async #retrieveActiveUser() {
        if (!this.#mutex.tryLock())
            throw new ResourceLockedError();

        try {
            const responseBody = await new Promise((resolve, reject) => {
                Dispatch.Get_UsersMe()
                    .onSuccess(body => resolve(body))
                    .onHttpError(() => resolve(null))
                    .onNetworkError(() => reject(new NetworkError()));
            });

            if (responseBody) {
                this.#activeUser.value = User.fromJSON(responseBody['user']);
                return true;
            }
        } finally {
            this.#mutex.unlock();
        }

        return false;
    }
};

export { SessionStore };