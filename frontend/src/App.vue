<template>
    <div
        v-show="fontsLoaded"
        ref="layoutEl"
        class="layout">
        <NavigationBar class="header" v-model:selected-tab="selectedTab" />

        <main class="main-content">
            <CanvasSection
                v-show="selectedTab == 'canvas'"
                role="tabpanel"
                :id="tabPanelIds.canvas"
                :revealRecipientSelect="revealRecipientSelect" />

            <FriendsSection
                v-show="selectedTab == 'friends'"
                role="tabpanel"
                :id="tabPanelIds.friends">
            </FriendsSection>
        </main>
    </div>
</template>

<script setup>
import {
    computed,
    provide,
    ref,
    useId,
    useTemplateRef,
    watch
} from 'vue';

import NavigationBar  from './components/NavigationBar.vue';
import CanvasSection  from './sections/CanvasSection.vue';
import FriendsSection from './sections/FriendsSection.vue';

import { DevicesStore } from './stores/devicesStore.js';
import { FriendsStore } from './stores/friendsStore.js';
import { SessionStore } from './stores/sessionStore.js';
import { Keys }         from './core/di/keys.js';

import { useElementIdRegistry } from './composables/useElementIdRegistry.js';
import { useWaitOnFont }        from './composables/useWaitOnFont.js';
import { useWaitOnTransition }  from './composables/useWaitOnTransition.js';

const tabPanelIds = useElementIdRegistry(Keys.AppTabPanelIdsRegistry, {
    canvas:  useId(),
    friends: useId()
});

const selectedTab = ref('canvas');
const session = new SessionStore();
const friends = new FriendsStore();
const devices = new DevicesStore();

const { isFontLoaded: isMainFontLoaded } = useWaitOnFont('--font-main');
const { isFontLoaded: isIconFontLoaded } = useWaitOnFont('iconfont');

const {
    isTransitionCompleted: isFadeInCompleted
} = useWaitOnTransition(useTemplateRef('layoutEl'), {
    propertyName: 'opacity',
    once:         true
});

const fontsLoaded = computed(() => {
    return isMainFontLoaded.value && isIconFontLoaded.value;
});

/*
    For performance reasons: don't run recipient select transition
    and content fade-in at same time.
*/
const revealRecipientSelect = computed(() => {
    return fontsLoaded.value && isFadeInCompleted.value;
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
</script>

<style scoped>
.layout {
    contain: content;

    align-content:  center;
    display:        flex;
    flex-flow:      column nowrap;
    padding-bottom: calc(var(--shadow-dist-l) * 2);

    transition:  opacity 300ms ease 200ms;

    & > .header       { contain: layout; z-index: 1; }
    & > .main-content { contain: layout; z-index: 0; }
}

.main-content {
    display:       grid;
    place-content: center;
}

@starting-style {
    .layout {
        opacity: 0;
    }
}
</style>
