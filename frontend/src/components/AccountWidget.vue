<template>
    <div
        v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
        class="account-widget account-widget--log-in">
        <UserButtonLogIn
            :is-pressed="isLoggingIn"
            @click="logIn" />
    </div>

    <div
        v-else-if="session.state.value === 'ready'"
        class="account-widget account-widget--profile">
        <AccountWidgetUserDrawerToggle
            ref="toggleEl"
            class="account-widget-user-drawer-toggle"
            :is-expanded="isExpanded"
            :aria-controls="dropdownMenuId"
            @click="isExpanded = !isExpanded" />

        <menu
            ref="userDrawerEl"
            class="user-drawer"
            :id="dropdownMenuId"
            :data-expanded="isExpanded"
            :inert="!isExpanded">
            <li>
                <UserButtonLogOut
                    ref="logOutButtonEl"
                    :is-pressed="isLoggingOut"
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

import AccountWidgetUsernameBadge    from './AccountWidgetUsernameBadge.vue';
import AccountWidgetUserDrawerToggle from './AccountWidgetUserDrawerToggle.vue';
import UserButtonLogIn               from './UserButtonLogIn.vue';
import UserButtonLogOut              from './UserButtonLogOut.vue';

import { useIsFocusWithin } from '@/composables/useIsFocusWithin.js';
import { Keys }             from '@/core/di/keys.js';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore);

const isExpanded   = ref(false);
const isLoggingOut = ref(false);
const isLoggingIn  = ref(false);

const toggleEl       = useTemplateRef('toggleEl');
const userDrawerEl   = useTemplateRef('userDrawerEl');
const logOutButtonEl = useTemplateRef('logOutButtonEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([() => toggleEl.value?.innerElement, userDrawerEl]);

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
    isLoggingOut.value = true;

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
    isLoggingIn.value = true;
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
        grid-template:
            "profile username"       auto
            "drawer  ."              auto /
            auto    minmax(0, 1fr);

        gap: 0 5px;
    }

    &.account-widget--log-in {
        display:     grid;
        place-items: center left;
    }

    & > .account-widget-user-drawer-toggle  { z-index: 1; grid-area: profile;  place-self: center; }
    & > .account-widget-username-badge      { z-index: 0; grid-area: username; place-self: center left; }
}

.account-widget-user-drawer-toggle:is(:hover, :active, :focus) ~ .user-drawer,
.user-drawer[data-expanded=true] {
    will-change: transform opacity;
}

.user-drawer {
    contain: layout;

    position: relative;
    
    pointer-events: none;
    scale:   0.8;
    opacity: 0;

    transition: 
        opacity 100ms,
        scale   100ms;
}

.user-drawer[data-expanded=true] {
    pointer-events: all;
    
    scale:   1;
    opacity: 1;
}

.user-drawer > * {
    position: absolute;
    top:      8px;
}
</style>