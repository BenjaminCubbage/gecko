import { computed, ref, reactive, readonly } from 'vue';

import { Device, DeviceStatus }             from '../models/Device.js';
import { Dispatch }           from '@/core/dispatch/Dispatch.js';
import { MultiResourceMutex } from './Mutex.js';
import { NetworkError, HttpError, ResourceLockedError } from './Errors.js';

class DevicesStore {
    constructor() {
        this._usersDevices = reactive(new Map());

        this._state = ref('uninitialized');
        this._mutex = new MultiResourceMutex();

        this._stateRO        = readonly(this._state);
        this._usersDevicesRO = readonly(this._usersDevices);

        this._anyDevices = computed(() => this._usersDevices.size > 0);
    }

    // 'uninitialized' | 'loggedout' | 'error' | 'loading' | 'loadingstatuses' | 'ready'
    state() {
        return this._stateRO;
    }

    usersDevices() {
        return this._usersDevicesRO;
    }

    anyDevices() {
        return this._anyDevices;
    }

    async resync(session, friends) {
        if (!session?.activeUser().value) {
            this._clearMap(this._usersDevices);
            this._state.value = 'loggedout';
            return;
        }

        if (!this._mutex.tryLockAll())
            throw new ResourceLockedError();

        try {
            this._state.value = 'loading';
            const userIDs = [
                session.activeUserID(),
                ...friends.activeFriends().map(f => f.user.userID)];
                
            this._clearMap(this._usersDevices);
            await this._resyncUsersDevices(userIDs);

            this._state.value = 'loadingstatuses';
            await this._resyncDeviceStatuses(this._usersDevices.values());

            this._state.value = 'ready';
        } catch (e) {
            this._state.value = 'error';
            throw e;
        } finally {
            this._mutex.unlockAll();
        }
    }

    async init(session, friends) {
        await this.resync(session, friends);
    }

    async updateFriends(session, friends) {
        const friendIDs = new Set(friends.map(f => f.user.userID));

        const added   = [...friendIDs.values()].filter(id => !this._usersDevices.has(id));
        const removed = [...this._usersDevices.keys()].filter(id => !friendIDs.has(id) && id != session.activeUserID());

        if (!this._mutex.tryLockMany(friendIDs.values()))
            throw new ResourceLockedError();

        for (const friendID of removed)
            this._usersDevices.delete(friendID);

        try {
            await this._resyncUsersDevices(added);
            await this._resyncDeviceStatuses(added
                .filter(id => this._usersDevices.has(id))
                .map(id => this._usersDevices.get(id)));
        } finally {
            this._mutex.unlockMany(friendIDs);
        }
    }

    async _resyncUsersDevices(userIDs) {
        let responses = await Promise.all(userIDs.map(id => this._fetchDevices(id)));

        for (let i = 0; i < responses.length; ++i) {
            const devices = responses[i]['devices'];

            if (!devices.length)
                continue;

            this._usersDevices.set(
                userIDs[i],
                responses[i]['devices'].map(json =>
                    Device.fromJSON(json, DeviceStatus.loading())));
        }
    }

    async _resyncDeviceStatuses(devices) {
        const inflight = [];

        for (const arr of devices) {
            for (const device of arr) {
                inflight.push({
                    device,
                    promise: this._fetchStatus(device.deviceID)
                });
            }
        }

        const responses = await Promise.allSettled(inflight.map(i => i.promise));

        for (let i = 0; i < inflight.length; ++i)
            switch (responses[i].status) {
                case 'fulfilled':
                    inflight[i].device.updateStatus(
                        DeviceStatus.fromJSON(responses[i].value['status']));
                    break;

                case 'rejected':
                    inflight[i].device.updateStatus(DeviceStatus.error());
                    break;
            }
    }

    async _fetchDevices(userID) {
        return await new Promise((resolve, reject) => {
            Dispatch.Get_UsersDevices(userID)
                .onSuccess     (body           => resolve(body))
                .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                .onNetworkError(()             => reject(new NetworkError()));
        });
    }

    async _fetchStatus(deviceID) {
        return await new Promise((resolve, reject) => {
            Dispatch.Get_DevicesStatus(deviceID)
                .onSuccess(body => resolve(body))
                .onError(()     => reject());
        });
    }

    _clearMap(map) {
        for (const key of map.keys())
            map.delete(key);
    }
}

export { DevicesStore };