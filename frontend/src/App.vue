<template>
    <div class="bg txtr-dots txtr-dots--blue">
        <Navbar class="navigation-bar" v-model:selectedTab="selectedTab" />

        <div class="front-and-center">
            <div v-show="selectedTab == 'canvas'">
                <RecipientSelect style="margin-bottom: 12px;" />
                <PicEditor />
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
import Navbar          from './components/navbar/Navbar.vue';
import PicEditor       from './components/pic_editor/PicEditor.vue';
import RecipientSelect from './components/recipient_select/RecipientSelect.vue';

import { FriendsStore } from '@/core/store/FriendsStore.js';
import { Keys }         from '@/core/store/Keys.js';
import { SessionStore } from '@/core/store/SessionStore.js';

const selectedTab = ref('canvas');
const session = new SessionStore();
const friends = new FriendsStore();

provide(Keys.SessionStore, session);
provide(Keys.FriendsStore, friends);

(async () => {
    await session.init();
    await friends.init(session);
})();

watch(session.state(), (newState, oldState) => {
    if ((newState === 'loggedout' && oldState === 'ready') ||
        (oldState === 'loggedout' && newState === 'ready')) {
        friends.resync();
    }
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
