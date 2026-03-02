<template>
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
                <DrawerButtonLogOut v-model:is-logging-out="isLoggingOut" @click="logOut" />
            </li>
        </menu>

        <AccountWidgetUsernameBadge class="account-widget-username-badge" />
    </div>
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

const isExpanded   = ref(false);
const isLoggingOut = ref(false);

const toggleEl = useTemplateRef('toggleEl');
const drawerEl = useTemplateRef('drawerEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([ () => toggleEl.value?.innerElement, drawerEl ]);

const dropdownMenuId = useId();

watch([isFocusWithinDrawer, isLoggingOut], () => {
    if (!isFocusWithinDrawer.value && !isLoggingOut.value)
        isExpanded.value = false;
});

async function logOut() {
    try {
        await session.requestLogOutAndReload();
    } catch (e) {
        let errorMessage;

        if (e instanceof HttpError)
            errorMessage = `Couldn't log out: Status ${e.status}`;
        else if (e instanceof NetworkError)
            errorMessage = `Couldn't log out: Connection failed`;
        else
            errorMessage = `Unexpected error while logging out`;

        snackBar.pushMessage(errorMessage);
        isLoggingOut.value = false;
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

.drawer-open-enter-active,
.drawer-open-leave-active {
    transform-origin: 50% 0;
    transition:
        scale   100ms ease,
        opacity 100ms ease;
}
</style>