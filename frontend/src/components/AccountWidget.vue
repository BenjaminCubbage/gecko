<template>
    <div
        v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
        class="account-widget account-widget--log-in">
        <DrawerButtonLogIn
            v-model:is-logging-in="isLoggingIn"
            @click="logIn" />
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
                <DrawerButtonLogOut
                    ref="logOutButtonEl"
                    v-model:is-logging-out="isLoggingOut"
                    @click="logOut" />
            </li>
        </menu>

        <AccountWidgetUsernameBadge class="account-widget-username-badge" />
    </div>
</template>

<script setup>
import {
    inject,
    nextTick,
    ref,
    useId,
    useTemplateRef,
    watch
} from 'vue';

import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

import DrawerButtonLogIn   from './DrawerButtonLogIn.vue';
import DrawerButtonLogOut  from './DrawerButtonLogOut.vue';
import DrawerButtonProfile from './DrawerButtonProfile.vue';

import { useIsFocusWithin } from '@/composables/useIsFocusWithin.js';
import { Keys }             from '@/core/di/keys.js';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore)

const isExpanded   = ref(false);
const isLoggingOut = ref(false);
const isLoggingIn  = ref(false);

const toggleEl       = useTemplateRef('toggleEl');
const drawerEl       = useTemplateRef('drawerEl');
const logOutButtonEl = useTemplateRef('logOutButtonEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([() => toggleEl.value?.innerElement, drawerEl]);

const dropdownMenuId = useId();

watch(isExpanded, async newValue => {
    if (newValue) {
        await nextTick();
        logOutButtonEl.value?.innerElement?.focus();
    }
});

watch([isFocusWithinDrawer, isLoggingOut], () => {
    if (!isFocusWithinDrawer.value && !isLoggingOut.value)
        isExpanded.value = false;
});

async function logOut() {
    try {
        await session.requestLogOutAndReload();
    } catch (e) {
        const errorMessage =
            e instanceof HttpError
                ? `Couldn't log out: Status ${e.status}`
                : e instanceof NetworkError
                    ? `Couldn't log out: Connection failed`
                    : `Unexpected error while logging out`;

        snackBar.pushMessage(errorMessage);
        isLoggingOut.value = false;
    }
}

function logIn() {
    if (!session.requestLogIn()) {
        snackBar.pushMessage('Already logged in');
        isLoggingIn.value = false;
    }
}
</script>

<style scoped>
.account-widget {
    &.account-widget--profile {
        display:     grid;
        flex-flow:   row nowrap;
        gap:         0 7px;

        grid-template:
            "profile username"       auto
            "drawer  ."              auto /
            auto    minmax(0, 1fr);
    }

    &.account-widget--log-in {
        display: grid;
    }
}

.drawer-button-profile {
    grid-area: profile;
}

.drawer-button-profile:is(:hover, :active, :focus) ~ .drawer,
.drawer[data-expanded=true] {
    will-change: transform opacity;
}

.account-widget-username-badge {
    grid-area: username;
}

.drawer {
    contain: layout;

    position: relative;
    
    pointer-events: none;
    scale:   0.8;
    opacity: 0;

    transition: 
        opacity 100ms,
        scale   100ms;
}

.drawer[data-expanded=true] {
    pointer-events: all;
    
    scale:   1;
    opacity: 1;
}

.drawer > * {
    position: absolute;
    top:      8px;
}
</style>