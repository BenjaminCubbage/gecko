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
                    aria-label-prev="Previous Friend"
                    aria-label-next="Next Friend"
                    :options="userOptions">
                    <template #label>
                        RECIPIENT
                    </template>

                    <template #option="{ option }">
                        {{ option.username }}
                    </template>
                </RecipientSelectCarousel>

                <RecipientSelectCarousel
                    v-model="selectedDevice"
                    class="devices"
                    variant="devices"
                    aria-label-prev="Previous Device"
                    aria-label-next="Next Device"
                    :options="deviceOptions">
                    <template #label>
                        DEVICE
                    </template>

                    <template #option="{ option }">
                        {{ option?.name }}
                    </template>
                </RecipientSelectCarousel>

                <svg class="arrow" viewBox="0 -10 39.4 49.5">
                    <path class="arrow-path st0" d="M30.9,15.8v-2.3h-2.3v-2.2h-2.3V9.1h-2.2c0,1.1,0,3.3,0,4.5c-3.6,0-7.6,0-11.2,0c0-2.8,0-6.2,0-8.9
                        c-2,0-4.8,0-6.7,0c0,4.3,0,9.1,0,13.4h2.3v2.3c5.1,0,10.6,0,15.7,0c0,1.1,0,3.3,0,4.5h2.3v-2.3h2.3v-2.3H31V18h2.3v-2.3L30.9,15.8
                        L30.9,15.8z"/>
                    <rect x="6.2" y="4.7" class="st1" width="2.3" height="13.4"/>
                    <rect x="17.4" y="9.1" transform="matrix(3.762241e-11 -1 1 3.762241e-11 3.7783 33.2783)" class="st1" width="2.3" height="11.2"/>
                    <rect x="9.5" y="3.6" transform="matrix(1.520923e-11 -1 1 1.520923e-11 4.879 16.484)" class="st1" width="2.3" height="4.5"/>
                    <rect x="24.1" y="9.1" class="st1" width="2.2" height="2.2"/>
                    <rect x="26.3" y="11.3" class="st1" width="2.3" height="2.2"/>
                    <rect x="28.6" y="13.5" class="st1" width="2.3" height="2.2"/>
                    <rect x="24.1" y="22.6" class="st2" width="2.3" height="2.3"/>
                    <rect x="26.4" y="20.3" class="st2" width="2.3" height="2.3"/>
                    <rect x="28.7" y="18" class="st2" width="2.3" height="2.3"/>
                    <rect x="31" y="15.7" class="st2" width="2.3" height="2.3"/>
                    <rect x="8.4" y="18" class="st2" width="15.7" height="2.3"/>
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
    contain:                  layout paint size;
    contain-intrinsic-height: 140px;

    align-items:   center;
    justify-items: stretch;
    display:       grid;
    margin:        0 var(--edge-margin-x);
}

.carousels {
    grid-template:
        "users users          ."       auto
        "arrow devices        devices" auto /
        60px   minmax(0, 1fr) 60px;

    display: grid;
    margin:  0 4%;

    .users   { place-self: end   stretch; }
    .devices { place-self: start stretch; }
    .arrow   { place-self: start end; }
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
    contain-intrinsic-size: 36px;

    grid-area: arrow;
    filter:    drop-shadow(0 var(--shadow-dist-s) black);

    translate: -12px 14px;

    & > .st0{ fill:var(--col-green-3); }
    & > .st1{ fill:var(--col-green-0); }
    & > .st2{ fill:var(--col-green-5); }
}

.arrow-path {
    stroke-width: 6px;
    stroke:       black;
    fill:         var(--col-green-3);
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