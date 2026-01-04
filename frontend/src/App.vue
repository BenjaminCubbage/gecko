<template>
    <div class="bg">
        <div class="dotted">
            <div class="header-strip-container">
                <Navbar class="navigation-bar" v-model:selectedTab="selectedTab" />
            </div>

            <div class="front-and-center" v-show="selectedTab == 'canvas'">
                <RecipientSelect style="margin-bottom: 4px;" />
                <PicEditor />
            </div>

            <div class="front-and-center" v-show="selectedTab == 'friends'">
                <FriendsList />
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, provide, computed, watch } from 'vue';
import FriendsList from './components/friends_list/FriendsList.vue'
import Navbar from './components/navbar/Navbar.vue';
import PicEditor from './components/pic_editor/PicEditor.vue';
import RecipientSelect from './components/recipient_select/RecipientSelect.vue';
import { FriendsStore } from '@/core/store/FriendsStore.js';
import { Keys } from '@/core/store/Keys.js';
import { SessionStore } from '@/core/store/SessionStore.js';

const selectedTab = ref('friends');
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

    console.log(newState, oldState);
});
</script>

<style scoped>
    :global(body), .bg {
        background: #4d7cfc;
    }

    .bg {
        height: 100dvh;
        display: grid;
    }

    .dotted {
        --dot-color: #3557b0;
        background-image: radial-gradient(circle at top,    var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at bottom, var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at right,  var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at left,   var(--dot-color) 2px, transparent 2px);

        background-size: 32px 32px;
        background-repeat: repeat;
        background-position-x: 50%;
        background-position-y: 0;
        
        scrollbar-gutter: stable both-edges;
        overflow-y: auto;
    }

    .navigation-bar {
        align-self: start;
    }

    .front-and-center {
        display: flex;
        flex-flow: column nowrap;
        gap: 4px;
        place-content: center;
        place-items: center;
    }

    .swap-screens-enter-active,
    .swap-screens-leave-active {
        transition: transform 120ms ease, opacity 120ms ease;
    }

    .swap-screens-enter-from {
        transform: translateX(-5px);
        opacity: 0;
    }

    .swap-screens-leave-to {
        transform: translateX(5px);
        opacity: 0;
    }
</style>
