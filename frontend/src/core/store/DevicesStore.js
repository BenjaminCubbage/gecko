import { ref, reactive, readonly } from 'vue';

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
    }

    // 'uninitialized' | 'loggedout' | 'error' | 'loading' | 'loadingstatuses' | 'ready'
    state() {
        return this._stateRO;
    }

    usersDevices() {
        return this._usersDevicesRO;
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
            const userIDs = [
                session.activeUserID(),
                ...friends.activeFriends().map(f => f.user.userID)];

            let inflight = [];

            this._state.value = 'loading';
            for (const userID of userIDs)
                inflight.push(new Promise((resolve, reject) => {
                    Dispatch.Get_UsersDevices(userID)
                        .onSuccess     (body           => resolve(body))
                        .onHttpError   ((body, status) => reject(new HttpError(status, body)))
                        .onNetworkError(()             => reject(new NetworkError()));
                }));

            let responses = await Promise.all(inflight);

            this._clearMap(this._usersDevices);
            for (let i = 0; i < inflight.length; ++i)
                this._usersDevices.set(
                    userIDs[i], 
                    responses[i]['devices'].map(json => Device.fromJSON(json, DeviceStatus.loading())));
                    
            this._state.value = 'loadingstatuses';
            inflight = [];

            for (const arr of this._usersDevices.values())
                for (const device of arr) {
                    inflight.push({ 
                        device, 
                        promise: new Promise((resolve, reject) => {
                            Dispatch.Get_DevicesStatus(device.deviceID)
                                .onSuccess(body => resolve(body))
                                .onError(() => reject());
                        }) 
                    });
                }

            responses = await Promise.allSettled(inflight.map(i => i.promise));

            for (let i = 0; i < inflight.length; ++i) {
                if (responses[i].status === 'fulfilled')
                    inflight[i].device.updateStatus(DeviceStatus.fromJSON(responses[i].value['status']));
                else
                    inflight[i].device.updateStatus(DeviceStatus.error());
            }

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

    _clearMap(map) {
        for (const key of map.keys())
            map.delete(key);
    }
}

export { DevicesStore };