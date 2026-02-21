<template>
    <div
        v-show="showContent"
        ref="layoutEl"
        class="layout">
        <NavigationBar class="navigation-bar" v-model:selectedTab="selectedTab" />

        <main class="main-content">
            <div v-show="selectedTab == 'canvas'">
                <RecipientSelect
                    :reveal="revealRecipientSelect"
                    @selectionChanged="selectedDeviceChanged" />
                <PicEditor v-show="false" :recipientDevice="selectedDevice" />
            </div>

            <div v-show="selectedTab == 'friends'">
                <FriendsList />
            </div>
        </main>
    </div>
</template>

<script setup>
import {
    computed,
    provide,
    ref,
    useTemplateRef,
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

import { useWaitOnFont }       from './composables/useWaitOnFont.js';
import { useWaitOnTransition } from './composables/useWaitOnTransition.js';

const selectedTab    = ref('canvas');
const selectedDevice = ref(null);
const session = new SessionStore();
const friends = new FriendsStore();
const devices = new DevicesStore();

const { isFontLoaded: isMainFontLoaded } = useWaitOnFont('--font-heading');
const { isFontLoaded: isIconFontLoaded } = useWaitOnFont('iconfont');

const {
    isTransitionCompleted: isFadeInCompleted
} = useWaitOnTransition(useTemplateRef('layoutEl'), {
    propertyName: 'opacity',
    once:         true
});

const showContent = computed(() => {
    return isMainFontLoaded.value && isIconFontLoaded.value;
});

/*
    For performance reasons: don't run recipient select transition
    and content fade-in at same time.
*/
const revealRecipientSelect = computed(() => {
    return showContent.value && isFadeInCompleted.value;
});

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
.layout {
    align-content: center;
    display:       flex;
    flex-flow:     column nowrap;
    margin-bottom: calc(var(--shadow-dist-l) * 2);

    transition:  opacity 300ms ease 200ms;
}

.main-content {
    display:       flex;
    flex-flow:     column nowrap;
    gap:           4px;
    place-content: center;
    place-items:   center;
}

@starting-style {
    .layout {
        opacity: 0;
    }
}
</style>
