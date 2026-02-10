<template>
    <div class="bg">
        <transition name="fade-in">
            <div v-show="isMainFontLoaded && isIconFontLoaded" class="flex-items">
                <NavigationBar class="navigation-bar" v-model:selectedTab="selectedTab" />

                <div class="front-and-center">
                    <div v-show="selectedTab == 'canvas'">
                        <RecipientSelect @selectionChanged="selectedDeviceChanged" />
                        <PicEditor :recipientDevice="selectedDevice" />
                    </div>

                    <div v-show="selectedTab == 'friends'">
                        <FriendsList />
                    </div>
                </div>
            </div>
        </transition>
    </div>
</template>

<script setup>
import { 
    provide, 
    ref, 
    watch
} from 'vue';

import FriendsList     from './components/FriendsList.vue';
import NavigationBar   from './components/NavigationBar.vue';
import PicEditor       from './components/PicEditor.vue';
import RecipientSelect from './components/RecipientSelect.vue';

import { DevicesStore } from './stores/devicesStore.js';
import { FriendsStore } from './stores/friendsStore.js';
import { SessionStore } from './stores/sessionStore.js';
import { Keys }         from './core/di/keys.js';

import { useWaitOnFont } from './composables/useWaitOnFont.js';

import { delay } from '@/core/async/delay.js';

const selectedTab    = ref('canvas');
const selectedDevice = ref(null);
const session = new SessionStore();
const friends = new FriendsStore();
const devices = new DevicesStore();

const { isFontLoaded: isMainFontLoaded } = useWaitOnFont('--font-heading');
const { isFontLoaded: isIconFontLoaded } = useWaitOnFont('iconfont');

provide(Keys.DevicesStore, devices);
provide(Keys.SessionStore, session);
provide(Keys.FriendsStore, friends);

(async () => {
    await session.requestResync();
    await friends.requestResync(session);
    await devices.requestResync(session, friends);
})();


watch(session.state, (newState, oldState) => {
    if ((newState === 'loggedout' && oldState === 'ready') ||
        (oldState === 'loggedout' && newState === 'ready')) {
        friends.requestResync(session);
        devices.requestResync(session, friends);
    }
});

watch(friends.activeFriends, newFriends => {
    if (devices.state.value === 'ready')
        devices.requestUpsertUserIDs(newFriends.map(f => f.user.userID));
});

function selectedDeviceChanged(value) {
    selectedDevice.value = value;
}
</script>

<style scoped>
.bg {
    height:           100dvh;
    overflow-y:       auto;
    scrollbar-gutter: stable both-edges;
}

.flex-items {
    align-content: center;
    display:       flex;
    flex-flow:     column nowrap;

    will-change: transform;
}

.front-and-center {
    display:       flex;
    flex-flow:     column nowrap;
    gap:           4px;
    place-content: center;
    place-items:   center;
}

.fade-in-enter-active {
    transition: opacity 200ms 200ms ease;
}

.fade-in-enter-from {
    opacity: 0;
}
</style>
