<template>
    <div class="recipient-select">
        <div class="carousels">
            <div class="user-carousel">
                <RecipientSelectCarousel
                    v-model="selectedUser"
                    :options="users"
                    :optionID="u => u.userID"
                    big
                    :mode="usersMode">
                    <template #label="{ option }">
                        {{ option.username }}
                    </template>
                </RecipientSelectCarousel>
            </div>

            <div class="device-carousel">
                <RecipientSelectCarousel
                    v-model="selectedDevice"
                    :options="userDevices"
                    :optionID="d => d.deviceID"
                    :signal="d => d.status"
                    :mode="devicesMode">
                    <template #label="{ option }">
                        {{ option?.name }}
                    </template>
                </RecipientSelectCarousel>
            </div>
        </div>
    </div>
</template>

<script setup>
import { computed, inject, ref, watch }   from 'vue';
import RecipientSelectCarousel  from './RecipientSelectCarousel.vue';
import { Keys }                 from '@/core/store/Keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
const devices = inject(Keys.DevicesStore);

const selectedUser   = ref(null);
const selectedDevice = ref(null);

const emit = defineEmits([ 'selectionChanged' ]);

watch(selectedDevice, newValue => emit('selectionChanged', newValue));

const usersMode = computed(() => {
    return {
        'uninitialized':   'loading',
        'loading':         'loading',
        'error':           'error',
        'ready':           'ready'
    }[friends.state().value];
});

const devicesMode = computed(() => {
    return {
        'uninitialized':   'loading',
        'loading':         'loading',
        'error':           'error',
        'loadingstatuses': 'ready',
        'ready':           'ready'
    }[devices.state().value];
});

const users = computed(() => {
    return friends.state().value === 'ready'
        ? [session.activeUser().value, ...friends.activeFriends().map(f => f.user)]
        : [];
});

const userDevices = computed(() => {
    return devices.state().value === 'ready'
        ? devices.usersDevices().get(selectedUser.value.userID)
        : [];
});
</script>

<style scoped>
.carousels {
    align-items: center;
    display:     flex;
    flex-flow:   column nowrap;
    gap:         6px;
}
</style>