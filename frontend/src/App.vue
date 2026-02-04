<template>
    <div class="bg txtr-dots txtr-dots--blue">
        <NavigationBar class="navigation-bar" v-model:selectedTab="selectedTab" />

        <div class="front-and-center">
            <div v-show="selectedTab == 'canvas'">
                <RecipientSelect @selectionChanged="d => selectedDevice = d" />
                <PicEditor :recipientDevice="selectedDevice" />
            </div>

            <div v-show="selectedTab == 'friends'">
                <FriendsList />
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, provide, watch } from 'vue';

import FriendsList     from './components/friends_list/FriendsList.vue'
import NavigationBar   from './components/navigation_bar/NavigationBar.vue';
import PicEditor       from './components/pic_editor/PicEditor.vue';
import RecipientSelect from './components/recipient_select/RecipientSelect.vue';

import { DevicesStore } from '@/stores/DevicesStore';
import { FriendsStore } from '@/stores/FriendsStore.js';
import { SessionStore } from '@/stores/SessionStore.js';
import { Keys }         from './core/di/Keys.js';

const selectedTab    = ref('canvas');
const selectedDevice = ref(null);
const session = new SessionStore();
const friends = new FriendsStore();
const devices = new DevicesStore();

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
</script>

<style scoped>
    :global(body) {
        background: #7298ff;
    }

    .bg {
        align-content:    center;
        display:          flex;
        flex-flow:        column nowrap;
        height:           100dvh;
        overflow-y:       auto;
        scrollbar-gutter: stable both-edges;
    }

    .front-and-center {
        display:       flex;
        flex-flow:     column nowrap;
        gap:           4px;
        place-content: center;
        place-items:   center;
    }
</style>
