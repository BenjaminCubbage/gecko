<template>
    <div class="recipient-select">
        <RecipientSelectLogInPrompt
            class="recipient-select-log-in-prompt"
            v-if="session.state.value === 'loggedout'"
            key="login" />

        <div
            v-else-if="session.state.value === 'ready'"
            class="carousels"
            key="carousels">
            <RecipientSelectSpinButton
                v-model="selectedUser"
                class="recipient-select-carousel--users"
                aria-label="Recipient"
                :options="userOptions"
                :get-option-label="getUserLabel">
                <template #label>
                    recipient
                </template>
            </RecipientSelectSpinButton>

            <RecipientSelectSpinButton
                v-model="selectedDevice"
                class="recipient-select-carousel--devices"
                variant="with-status"
                aria-label="Recipient's Device Suboption"
                :options="deviceOptions"
                :get-option-label="getDeviceLabel"
                :get-option-status-label="getDeviceStatusLabel">
                <template #label>
                    device
                </template>
            </RecipientSelectSpinButton>

            <svg class="arrow" viewBox="0 -10 39.4 49.5">
                <path class="arrow-path st0" d="M30.9,15.8v-2.3h-2.3v-2.2h-2.3V9.1h-2.2c0,1.1,0,3.3,0,4.5c-3.6,0-7.6,0-11.2,0c0-2.8,0-6.2,0-8.9
                    c-2,0-4.8,0-6.7,0c0,4.3,0,9.1,0,13.4h2.3v2.3c5.1,0,10.6,0,15.7,0c0,1.1,0,3.3,0,4.5h2.3v-2.3h2.3v-2.3H31V18h2.3v-2.3L30.9,15.8
                    L30.9,15.8z"/>
                <rect x="6.2" y="4.7" class="st1" width="2.3" height="13.4"/>
                <rect x="17.4" y="9.1" transform="matrix(0 -1 1 0 3.7783 33.2783)" class="st1" width="2.3" height="11.2"/>
                <rect x="9.5" y="3.6" transform="matrix(0 -1 1 0 4.879 16.484)" class="st1" width="2.3" height="4.5"/>
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
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    watch
} from 'vue';

import RecipientSelectSpinButton    from './RecipientSelectSpinButton.vue';
import RecipientSelectLogInPrompt from './RecipientSelectLogInPrompt.vue';
import { Keys }                   from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
const devices = inject(Keys.DevicesStore);

const selectedUser   = ref();
const selectedDevice = ref();

const emit = defineEmits(['selectionChanged']);

const userOptions = computed(() => {
    return devices.state.value === 'ready'
        ? [session.activeUser.value, ...friends.activeFriends.value.map(f => f.user)]
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

function getUserLabel(user)   { 
    return session?.activeUserID !== user?.userID
        ? user?.username
        : `${user?.username} [me]`;
}
function getDeviceLabel(device) { return device?.name; }

const deviceStatusLabels = {
    'online': {
        color:    'green',
        text:     'online',
        ariaText: 'online status'
    },

    'offline': {
        color:    'red',
        text:     'offline',
        ariaText: 'offline status'
    },

    'pending': {
        color:    'blue',
        text:     'pending',
        ariaText: 'pending connection status'
    },

    'loading': null
};

function getDeviceStatusLabel(device) {
    return deviceStatusLabels[device?.status] ?? deviceStatusLabels['loading'];
}
</script>

<style scoped>
.recipient-select {
    contain: layout paint size;

    align-items:   center;
    display:       grid;
    height:        140px;

    & > .carousels                      { place-self: stretch; }
    & > .recipient-select-log-in-prompt { place-self: center; }
}

.carousels {
    grid-template:
        "users users          ."       auto
        "arrow devices        devices" auto /
        60px   minmax(0, 1fr) 60px;

    display: grid;
    margin:  0 4%;

    .recipient-select-carousel--users   { grid-area: users;   place-self: end   stretch; font-size: 2.4rem; }
    .recipient-select-carousel--devices { grid-area: devices; place-self: start stretch; font-size: 2.2rem; }
    .arrow                              { grid-area: arrow;   place-self: start end; }
}

.arrow {
    contain: strict;

    height: 36px;
    width:  36px;

    filter:
        drop-shadow(0 var(--shadow-dist-s) black);

    translate: -12px 13px;

    & > .st0{ fill: var(--col-green-3); }
    & > .st1{ fill: var(--col-green-0); }
    & > .st2{ fill: var(--col-green-4); }

    & > .st0{
        stroke-width: 6.5px;
        stroke:       black;
    }
}
</style>