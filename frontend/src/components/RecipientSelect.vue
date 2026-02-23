<template>
    <div class="recipient-select">
        <transition name="slide-up" mode="out-in">
            <RecipientSelectLogInButton
                v-if="reveal && session.state.value === 'loggedout'"
                key="login" />

            <div
                v-else-if="reveal && session.state.value === 'ready' && selectedDevice != null"
                class="carousels"
                key="carousels">
                <RecipientSelectCarousel
                    v-model="selectedUser"
                    class="users"
                    variant="users"
                    :options="userOptions">
                    <template #label="{ option }">
                        {{ option.username }}
                    </template>
                </RecipientSelectCarousel>

                <RecipientSelectCarousel
                    v-model="selectedDevice"
                    class="devices"
                    variant="devices"
                    :options="deviceOptions">
                    <template #label="{ option }">
                        {{ option?.name }}
                    </template>
                </RecipientSelectCarousel>

                <svg class="arrow" viewBox="0 -10 39.4 49.5">
                    <path class="arrow-path" d="M35.5,16.3v-3.2h-3.2V10h-3.2V6.9H26c0,1.6,0,4.7,0,6.3c-5.1,0-10.7,0-15.8,0c0-4,0-8.7,0-12.6c-2.8,0-6.7,0-9.5,0
                        c0,6.1,0,12.8,0,18.9h3.2v3.2c7.2,0,14.9,0,22.1,0c0,1.6,0,4.7,0,6.3h3.2v-3.2h3.2v-3.2h3.2v-3.2h3.2v-3.2H35.5z" />
                </svg>
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

import RecipientSelectCarousel    from './RecipientSelectCarousel.vue';
import RecipientSelectLogInButton from './RecipientSelectLogInButton.vue';
import { Keys }                   from '@/core/di/keys.js';

const props = defineProps({
    reveal: {
        type:    Boolean,
        default: false
    }
});

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

watch(selectedDevice, newValue => emit('selectionChanged', newValue));
</script>

<style scoped>
.recipient-select {
    align-items:   center;
    justify-items: stretch;
    display:       grid;
    height:        120px;
    margin:        0 var(--edge-margin-x);
}

.carousels {
    grid-template:
        "users users          ."       auto
        "arrow devices        devices" auto /
        60px   minmax(0, 1fr) 60px;

    margin: 0 45px;

    align-items:   center;
    justify-items: stretch;
    display:       grid;
    flex-flow:     column nowrap;
    gap:           8px 0;
}

.users {
    grid-area: users;
    font-size: 2.4rem;
}

.devices {
    grid-area: devices;
    font-size: 2.2rem;
}

.arrow {
    contain:                strict;
    contain-intrinsic-size: 28px;

    grid-area: arrow;
    color:     white;
    filter:    drop-shadow(0 2px white);

    translate: 0 -2px;
}

.arrow-path {
    stroke-width: 7px;
    stroke:       white;
    fill:         black;
    paint-order:  stroke;
}

.slide-up-enter-active,
.slide-up-leave-active {
    transition: scale 300ms ease;
    transform-origin: 50% 100%;
}

.slide-up-enter-from,
.slide-up-leave-to {
    scale: 0
}
</style>