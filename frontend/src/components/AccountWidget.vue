<template>
    <transition name="login-appear" mode="out-in">
        <div
            v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
            class="account-widget account-widget--login">
            <AccountWidgetLogInButton />
        </div>

        <div
            v-else-if="session.state.value === 'ready'"
            class="account-widget account-widget--profile">
            <DrawerButtonProfile
                ref="toggleEl"
                v-model="isExpanded"
                class="drawer-button-profile"
                :aria-controls="dropdownMenuId" />

            <menu
                ref="drawerEl"
                class="drawer"
                :id="dropdownMenuId"
                :data-expanded="isExpanded"
                :inert="!isExpanded">
                <li>
                    <DrawerButtonLogOut @click="logOut" />
                </li>
            </menu>

            <AccountWidgetUsernameBadge class="account-widget-username-badge" />
        </div>
    </transition>
</template>

<script setup>
import {
    inject,
    ref,
    useId,
    useTemplateRef,
    watch
} from 'vue';

import AccountWidgetLogInButton   from './AccountWidgetLogInButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

import DrawerButtonProfile from './DrawerButtonProfile.vue';
import DrawerButtonLogOut  from './DrawerButtonLogOut.vue';

import { useIsFocusWithin } from '@/composables/useIsFocusWithin';

import { Keys } from '@/core/di/keys.js';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore)

const isExpanded = ref(false);

const toggleEl = useTemplateRef('toggleEl');
const drawerEl = useTemplateRef('drawerEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([ () => toggleEl.value?.innerElement, drawerEl ]);

const dropdownMenuId = useId();

watch(isFocusWithinDrawer, newValue => {
    if (!newValue)
        isExpanded.value = false;
});

async function logOut() {
    try {
        session.requestLogOut();
        snackBar.pushMessage('You are now logged out');
    } catch (e) {
        let errorMessage;

        if (e instanceof HttpError)
            errorMessage = `Couldn't log out: Error ${e.status}`;
        else if (e instanceof NetworkError)
            errorMessage = `Couldn't log out: Couldn't connect`;
        else
            errorMessage = `Unexpected error while logging out`;

        snackBar.pushMessage(errorMessage);
    }
}
</script>

<style scoped>
.account-widget {
    &.account-widget--profile {
        display:     grid;
        flex-flow:   row nowrap;
        gap:         8px 7px;

        grid-template:
            "profile username"       auto
            "drawer  ."              auto /
            auto    minmax(0, 1fr);
    }

    &.account-widget--login {
        display: grid;
    }
}

.drawer-button-profile {
    grid-area: profile;
}

.drawer-button-profile:is(:hover, :active, :focus) + .drawer,
.drawer[data-expanded=true] {
    will-change: transform;
}

.account-widget-username-badge {
    grid-area: username;
}

.drawer {
    contain: layout;

    position: relative;
    pointer-events: none;

    scale:            0;
    transform-origin: 50% 0;
    transition:       scale 200ms ease;
}

.drawer[data-expanded=true] {
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
</style>