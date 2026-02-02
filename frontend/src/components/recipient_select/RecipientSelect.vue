<template>
    <div class="recipient-select">
        <transition name="callout-expand" mode="out-in">
            <div class="callout" v-if="showCallout">
                <RecipientSelectCallout>
                    {{ calloutText }}
                </RecipientSelectCallout>
            </div>

            <div class="carousels" v-else>
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
        </transition>
    </div>
</template>

<script setup>
import { computed, inject, ref, watch }   from 'vue';
import RecipientSelectCarousel  from './RecipientSelectCarousel.vue';
import RecipientSelectCallout from './RecipientSelectCallout.vue';
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
    return usersMode.value === 'ready'
        ? [session.activeUser().value, ...friends.activeFriends().map(f => f.user)]
            .filter(user => devices.usersDevices().has(user.userID))
        : [];
});

const userDevices = computed(() => {
    return devicesMode.value === 'ready' && 
           selectedUser.value && 
           devices.usersDevices().has(selectedUser.value.userID)
        ? devices.usersDevices().get(selectedUser.value.userID)
        : [];
});

const showCallout = computed(() => devices.state().value !== 'ready' || !devices.anyDevices().value);
const calloutText = computed(() => {
    if (session.state().value === 'loading')
        return 'Loading...';

    if (session.state().value === 'loggedout')
        return 'Not logged in';

    if (devices.state().value === 'error')
        return 'Error loading devices';

    if (devices.state().value !== 'ready')
        return 'Loading...';

    if (!devices.anyDevices().value)
        return 'No friends with any devices';
});
</script>

<style scoped>
.recipient-select {
    display:     grid;
    height:      100px;
    place-items: center;
}

.callout {
    grid-area:   1 / 1;
    will-change: transform;
}

.carousels {
    align-items: center;
    display:     flex;
    flex-flow:   column nowrap;
    gap:         6px;
    grid-area:   1 / 1;
    will-change: transform;
}

.callout-expand-enter-active,
.callout-expand-leave-active {
    transition: transform 50ms ease;
}

.callout-expand-enter-from,
.callout-expand-leave-to {
    transform: scale(0.9);
}
</style>