<template>
    <div class="recipient-select">
        <div class="carousels">
            <div class="user-carousel">
                <RecipientSelectCarousel
                    v-model="selectedUser"
                    :options="users"
                    :optionID="u => u.userID"
                    big
                    :mode="userMode">
                    <template #label="{ option }">
                        {{ option['username'] }}
                    </template>
                </RecipientSelectCarousel>
            </div>

            <div class="device-carousel">
                <RecipientSelectCarousel
                    v-model="selectedDevice"
                    :options="devices"
                    :optionID="d => d.deviceID"
                    :signal="m => m.signalStatus"
                    :mode="deviceMode">
                    <template #label="{ option }">
                        {{ option.name }}
                    </template>
                </RecipientSelectCarousel>
            </div>
        </div>
    </div>
</template>

<script setup>
import { inject, ref, watch } from 'vue';
import RecipientSelectCarousel from './RecipientSelectCarousel.vue';
import { Dispatch } from '../../core/dispatch/Dispatch';
import { Keys } from '@/core/store/Keys.js';

const session = inject(Keys.SessionStore);
const userMode   = ref('loading');
const deviceMode = ref('loading');

const users = ref([]);
const devices = ref([]);

const selectedDevice = ref(null);
const selectedUser   = ref(null);

watch(session.activeUser(), user => {
    if (!user)
        return;

    users.value[0]     = user;
    selectedUser.value = user;
    userMode.value = 'ready';

    // note(ben): Cheap hack for now.
    if (devices.value.length)
        return;

    Dispatch.Get_UsersDevices(session.activeUserID())
        .onSuccess(body => {
            devices.value = body.devices.map(d => ({
                deviceID:     d['device_id'],
                name:         d['name'],
                signalStatus: 'loading'
            }));

            selectedDevice.value = devices.value[0];
            deviceMode.value = 'ready';

            for (const d of devices.value)
                Dispatch.Get_DevicesStatus(d.deviceID)
                    .onSuccess(body => {
                        d.signalStatus = body['status'];
                    });
        });
}, { immediate: true });
</script>

<style scoped>
.carousels {
    align-items: center;
    display:     flex;
    flex-flow:   column nowrap;
    gap:         6px;
}
</style>