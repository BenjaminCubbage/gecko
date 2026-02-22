<template>
    <div class="account-widget">
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
                    class="profile-button" />

                <transition name="drawer-open">
                    <div
                        v-show="isExpanded"
                        ref="drawerEl"
                        class="drawer">
                        <DrawerButtonLogOut @click="logOut" />
                    </div>
                </transition>

                <AccountWidgetUsernameBadge class="username-badge" />
            </div>
        </transition>
    </div>
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
.account-widget {
    display: flex;
}

.widget-layout {
    align-items: stretch;
    display:     grid;
    flex-flow:   row nowrap;
    gap:         8px 7px;

    grid-template-areas:
        "profile username"
        "drawer  .";
}

.profile-button {
    grid-area: profile;
}

.username-badge {
    grid-area: username;
}

.login-button-wrapper {
    display: grid;
}

.drawer {
    position: relative;
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
    scale: 0.8;
    opacity: 0;
}
</style>