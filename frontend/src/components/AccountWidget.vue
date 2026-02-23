<template>
    <transition name="login-appear" mode="out-in">
        <div
            v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
            class="login-button-wrapper">
            <AccountWidgetLogInButton />
        </div>

        <div
            v-else-if="session.state.value === 'ready'"
            class="widget-layout">
            <DrawerButtonProfile
                ref="toggleEl"
                v-model="isExpanded"
                class="drawer-toggle" />

            <div
                ref="drawerEl"
                class="drawer"
                :class="{ 'drawer--expanded': isExpanded }"
                :inert="!isExpanded">
                <DrawerButtonLogOut @click="logOut" />
            </div>

            <AccountWidgetUsernameBadge class="username-badge" />
        </div>
    </transition>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import AccountWidgetLogInButton   from './AccountWidgetLogInButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

import DrawerButtonProfile from './DrawerButtonProfile.vue';
import DrawerButtonLogOut  from './DrawerButtonLogOut.vue';

import { useIsFocusWithin } from '@/composables/useIsFocusWithin';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

const isExpanded = ref(false);

const toggleEl = useTemplateRef('toggleEl');
const drawerEl = useTemplateRef('drawerEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([ () => toggleEl.value?.innerElement, drawerEl ]);

watch(isFocusWithinDrawer, newValue => {
    if (!newValue)
        isExpanded.value = false;
});

async function logOut() {
    session.requestLogOut();
}
</script>

<style scoped>
.widget-layout {
    display:     grid;
    flex-flow:   row nowrap;
    gap:         8px 7px;

    grid-template:
        "profile username"       auto
        "drawer  ."              auto /
         auto    minmax(0, 1fr);
}

.drawer-toggle {
    grid-area: profile;
}

.drawer-toggle:is(:hover, :active, :focus) + .drawer,
.drawer--expanded {
    will-change: transform;
}

.username-badge {
    grid-area: username;
}

.login-button-wrapper {
    display: grid;
}

.drawer {
    contain: layout;

    position: relative;
    pointer-events: none;

    scale:            0;
    transform-origin: 50% 0;
    transition:       scale 200ms ease;
}

.drawer--expanded {
    pointer-events: all;
    scale:          1;
}

.drawer > * {
    position: absolute;
}

.login-appear-leave-active {
    transition:
        transform 200ms ease-in,
        opacity   200ms ease-in;
}

.login-appear-enter-active {
    transition:
        transform 200ms ease-out,
        opacity   200ms ease-out;
}

.login-appear-enter-from,
.login-appear-leave-to {
    transform: translateY(-140px);
}

.drawer-open-enter-active,
.drawer-open-leave-active {
    transform-origin: 50% 0;
    transition:
        scale   100ms ease,
        opacity 100ms ease;
}

.drawer-open-enter-from,
.drawer-open-leave-to {
}
</style>