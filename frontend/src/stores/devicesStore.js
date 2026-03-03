import {
    computed,
    reactive,
    readonly,
    ref
} from 'vue';

import {
    NetworkError,
    HttpError,
    PartialCompletionError,
    ResourceLockedError
} from '@/core/errors/errors.js';

import { Device }             from '@//models/device.js';
import { Dispatch }           from '@/core/dispatch/Dispatch.js';
import { MultiResourceMutex } from '@/core/async/mutex.js';

/*
    Stores owned devices keyed by user ID.
*/
export class DevicesStore {
    #deviceOwners;
    #mutex;
    #state;

    #deviceOwnersRO;
    #stateRO;

    #hasAnyDevices;

    constructor() {
        this.#deviceOwners = reactive(Object.create(null));

        this.#mutex = new MultiResourceMutex();
        this.#state = ref('uninitialized');

        this.#deviceOwnersRO = readonly(this.#deviceOwners);
        this.#stateRO        = readonly(this.#state);

        this.#hasAnyDevices = computed(() =>
            DevicesStore.#getHasAnyDevices(this.#deviceOwners));
    }

    get deviceOwners() {
        return this.#deviceOwnersRO;
    }

    get state() {
        return this.#stateRO;
    }

    get hasAnyDevices() {
        return this.#hasAnyDevices;
    }

    /*
        Rebuild map from the given session and friends list.
    */
    async requestResync(session, friends) {
        if (!this.#mutex.tryLockAll())
            throw new ResourceLockedError();

        try {
            DevicesStore.#clearMap(this.#deviceOwners);

            if (!session?.activeUser.value) {
                this.#state.value = 'loggedout';
                this.#mutex.unlockAll();
                return;
            }

            this.#state.value = 'loading';
            await this.#requestUpsertDeviceOwners(
                DevicesStore.#getIDs(session, friends));
            this.#state.value = 'ready';

            try {
                await this.#requestDeviceStatuses(this.#deviceOwners);
            } catch (e) {
                throw new PartialCompletionError(e);
            }
        } catch (e) {
            if (!(e instanceof PartialCompletionError))
                this.#state.value = 'error';

            throw e;
        } finally {
            this.#mutex.unlockAll();
        }
    }

    /*
        Fetch devices associated with the user IDs and update the
        local cache.
    */
    async requestUpsertUserIDs(userIDs) {
        if (!this.#mutex.tryLockMany(userIDs))
            throw new ResourceLockedError();

        try {
            await this.#requestUpsertDeviceOwners(userIDs);

            try {
                await this.#requestDeviceStatuses(
                    userIDs.map(id => this.#deviceOwners[id]));
            } catch (e) {
                throw new PartialCompletionError(e);
            }
        } finally {
            this.#mutex.unlockMany(userIDs);
        }
    }

    async #requestUpsertDeviceOwners(userIDs) {
        const responses = await Promise.all(userIDs.map(id => this.#fetchDevices(id)));

        for (let i = 0; i < responses.length; ++i) {
            const devices = responses[i]['devices'];

            if (!devices.length)
                continue;

            this.#deviceOwners[userIDs[i]] =
                responses[i]['devices'].map(json => Device.fromJSON(json));
        }
    }

    async #requestDeviceStatuses(deviceOwners) {
        const inflight = [];

        for (const device of DevicesStore.#getDevices(deviceOwners)) {
            inflight.push({
                device,
                promise: this.#fetchStatus(device.deviceID)
            });
        }

        const responses = await Promise.allSettled(inflight.map(i => i.promise));

        for (let i = 0; i < inflight.length; ++i) {
            switch (responses[i].status) {
            case 'fulfilled':
                inflight[i].device.applyStatusJSON(responses[i].value['status']);
                break;

            case 'rejected':
                inflight[i].device.applyStatusError(`Couldn't fetch device status`);
                break;
            }
        }
    }

    async #fetchDevices(userID) {
        return await new Promise((resolve, reject) => {
            Dispatch.Get_UsersDevices(userID)
                .onSuccess     (body           => resolve(body))
                .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                .onNetworkError(()             => reject(new NetworkError()));
        });
    }

    async #fetchStatus(deviceID) {
        return await new Promise((resolve, reject) => {
            Dispatch.Get_DevicesStatus(deviceID)
                .onSuccess(body => resolve(body))
                .onError(()     => reject());
        });
    }

    static #clearMap(object) {
        for (const key in object)
            delete object[key];
    }

    static #getIDs(session, friends) {
        return [
            session.activeUserID,
            ...friends.activeFriends.map(f => f.user.userID)];
    }

    static #getDevices(deviceOwners) {
        const result = [];
        for (const userID in deviceOwners)
            result.push(...deviceOwners[userID]);

        return result;
    }

    static #getHasAnyDevices(deviceOwners) {
        for (const userID in deviceOwners)
            if (deviceOwners[userID].length > 0)
                return true;

        return false;
    }
}