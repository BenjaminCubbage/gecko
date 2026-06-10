<template>
    <div
        v-if="
            session.state.value === 'loggedout' ||
            session.state.value === 'error'"
        class="
            account-widget account-widget--log-in
            shdw-before shdw-before--inst-lt-gray shdw-before--elevated-s">
        <AccountWidgetLogInButton
            :is-pressed="isLoggingIn"
            @click="logIn" />
    </div>

    <div
        v-else-if="session.state.value === 'ready'"
        class="
            account-widget account-widget--profile
            shdw-before shdw-before--inst-lt-gray shdw-before--elevated-s">
        <AccountWidgetUserOptsToggle
            ref="toggleEl"
            :is-expanded="isExpanded"
            @click="isExpanded = !isExpanded" />

        <UserOptsPopover
            v-model:is-expanded="isExpanded"
            :popoverTarget="toggleEl?.innerElement"
            @log-out="logOut" />

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

import AccountWidgetUsernameBadge  from './AccountWidgetUsernameBadge.vue';
import AccountWidgetUserOptsToggle from './AccountWidgetUserOptsToggle.vue';
import AccountWidgetLogInButton    from './AccountWidgetLogInButton.vue';
import UserOptsPopover             from './UserOptsPopover.vue';

import { Keys } from '@/core/di/keys.js';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

const session  = inject(Keys.SessionStore);
const toggleEl = useTemplateRef('toggleEl');

const isExpanded  = ref(false);
const isLoggingIn = ref(false);

async function logOut(resolve) {
    try {
        await session.requestLogOutAndReload();
    } catch (e) {
        const errorMessage =
            e instanceof HttpError
                ? `Couldn't log out: Status ${e.status}`
                : e instanceof NetworkError
                    ? `Couldn't log out: Connection failed`
                    : `Unexpected error while logging out`;
        console.error(errorMessage);
        resolve();
    }
}

function logIn() {
    isLoggingIn.value = true;
    if (!session.requestLogIn())
        isLoggingIn.value = false;
}
</script>

<style scoped>
.account-widget {
    position: relative;

    display:     flex;
    align-items: center;

    gap: 8px;
}
</style>