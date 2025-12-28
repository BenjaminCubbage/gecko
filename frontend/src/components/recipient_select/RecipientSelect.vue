<template>
    <div class="recipient-select">
        <div class="carousels">
            <div class="user-carousel">
                <RecipientSelectCarousel 
                    v-model="selectedUser" 
                    :options="users" 
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

const session = inject('session');
const userMode   = ref('loading');
const deviceMode = ref('loading');

const users = ref([]);

const devices = ref([]);

watch(session.value, () => {
    if (!session.value.activeUser())
        return;

    users.value[0] = session.value.activeUser().json()

    selectedUser.value = users.value[0];
    userMode.value = 'loaded';

    // note(ben): Cheap hack for now.
    if (devices.value.length)
        return;

    Dispatch.Get_UsersDevices(session.value.activeUser().json()['user_id'])
        .onSuccess(body => {
            devices.value = body.devices.map(d => ({
                deviceID:     d['device_id'],
                name:         d['name'],
                signalStatus: 'loading'
            }));

            selectedDevice.value = devices.value[0];
            deviceMode.value = 'loaded';

            for (let d of devices.value)
                Dispatch.Get_DevicesStatus(d.deviceID)
                    .onSuccess(body => {
                        d.signalStatus = body['status'];
                    });
        });
});

const selectedDevice = ref(null);
const selectedUser   = ref(null);
</script>

<style scoped>
    .carousels {
        display: flex;
        flex-flow: column nowrap;
        gap: 6px;

        align-items: center;
    }
</style>