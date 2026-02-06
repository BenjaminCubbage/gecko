<template>
    <div class="recipient-select">
        <transition name="slide-up" mode="out-in">
            <RecipientSelectCallout
                v-if="showCallout"
                class="callout"
                key="callout">
                {{ calloutText }}
            </RecipientSelectCallout>

            <div
                v-else
                class="carousels"
                key="carousels">
                <RecipientSelectCarousel
                    v-model="selectedUser"
                    variant="users"
                    :options="userOptions">
                    <template #label="{ option }">
                        {{ option.username }}
                    </template>
                </RecipientSelectCarousel>

                <RecipientSelectCarousel
                    v-model="selectedDevice"
                    variant="devices"
                    :options="deviceOptions">
                    <template #label="{ option }">
                        {{ option?.name }}
                    </template>
                </RecipientSelectCarousel>
            </div>
        </transition>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    watch
} from 'vue';

import RecipientSelectCarousel from './RecipientSelectCarousel.vue';
import RecipientSelectCallout  from './RecipientSelectCallout.vue';
import { Keys }                from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
const devices = inject(Keys.DevicesStore);

const selectedUser   = ref();
const selectedDevice = ref();

const emit = defineEmits([ 'selectionChanged' ]);

const userOptions = computed(() => {
    return devices.state.value === 'ready'
        ? [session.activeUser.value, ...friends.activeFriends.map(f => f.user)]
            .filter(user => user.userID in devices.deviceOwners)
        : [];
});

const deviceOptions = computed(() => {
    return selectedUser.value != null
        ? devices.deviceOwners[selectedUser.value.userID]
        : [];
});

watch(userOptions, () => {
    if (!userOptions.value.includes(selectedUser))
        selectedUser.value = userOptions.value[0];
});

watch(deviceOptions, () => {
    if (!deviceOptions.value.includes(selectedDevice))
        selectedDevice.value = deviceOptions.value[0];
});

const showCallout = computed(() => {
    return selectedDevice.value == null;
});

const calloutText = computed(() => {
    switch (devices.state.value) {
        case 'loggedout':
            return 'Not logged in';

        case 'error':
            return 'Error loading devices';

        case 'ready':
            return devices.hasAnyDevices.value
                ? 'Loading...'
                : 'No friends with any devices';
    }

    return 'Loading...';
});

watch(selectedDevice, newValue => emit('selectionChanged', newValue));
</script>

<style scoped>
.recipient-select {
    display:     grid;
    height:      100px;
    place-items: center;
}

.callout {
    grid-area:   1 / 1;
}

.carousels {
    align-items: center;
    display:     flex;
    flex-flow:   column nowrap;
    gap:         6px;
    grid-area:   1 / 1;
}

.slide-up-enter-active,
.slide-up-leave-active {
    transition: 
        transform 50ms ease-out;
}

.slide-up-enter-from,
.slide-up-leave-to {
  transform: scale(0.8);
}
</style>