<template>
    <div class="recipient-select">
        <transition name="slide-up" mode="out-in">
            <RecipientSelectLogInButton
                v-if="show && session.state.value === 'loggedout'"
                key="login" />

            <div
                v-else-if="show && session.state.value === 'ready' && selectedDevice != null"
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

import RecipientSelectCarousel    from './RecipientSelectCarousel.vue';
import RecipientSelectLogInButton from './RecipientSelectLogInButton.vue';
import { Keys }                   from '@/core/di/keys.js';

const props = defineProps({
    show: {
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
    display:     grid;
    height:      100px;
    place-items: center;
}

.carousels {
    align-items: center;
    display:     flex;
    flex-flow:   column nowrap;
    gap:         6px;
}

.slide-up-enter-active,
.slide-up-leave-active {
    transition: 
        transform 200ms ease;
}

.slide-up-enter-from,
.slide-up-leave-to {
  transform: translateY(100px);
}
</style>