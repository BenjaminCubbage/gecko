<template>
    <div class="header-strip">
        <transition name="login-appear" mode="out-in">
            <AccountWidgetLogInButton
                v-if="!session.activeUser.value"
                href="/auth/login" />

            <div v-else>
                <div class="badge-section">
                    <AccountWidgetUsernameBadge
                        :username="session.activeUser.value['username']"
                        :status="userBadgeStatus"
                        :forbiddenUsernames="forbiddenUsernames"
                        @requestEdit="userBadgeStatus = 'editing'"
                        @cancel="userBadgeStatus = 'normal'"
                        @submit="changeUsername" />

                    <AccountWidgetLogOutButton
                        v-show="userBadgeStatus == 'normal'"
                        @click="logout" />
                </div>

                <AccountWidgetStatusBubble ref="statusBubble" />
            </div>
        </transition>
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef
} from 'vue';

import AccountWidgetLogInButton   from './AccountWidgetLogInButton.vue';
import AccountWidgetLogOutButton  from './AccountWidgetLogOutButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';
import AccountWidgetStatusBubble  from './AccountWidgetStatusBubble.vue';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

import { errorResponseToDisplayString } from '@/core/http/errorResponseToDisplayString.js';
import { Keys }                         from '@/core/di/keys.js';

const statusBubble = useTemplateRef('statusBubble');

const session = inject(Keys.SessionStore);

const userBadgeStatus    = ref('normal');
const forbiddenUsernames = ref([]);

async function changeUsername(newUsername) {
    userBadgeStatus.value = 'loading';

    try {
        await session.changeUsername(newUsername);

        userBadgeStatus.value = 'normal';
    } catch (e) {
        if (e instanceof HttpError)
            statusBubble.value.showMessage(errorResponseToDisplayString(e.body));
        else if (e instanceof NetworkError)
            statusBubble.value.showMessage(`Couldn't connect to the server!`);

        userBadgeStatus.value = 'editing';
    }
}

async function logout() {
    session.requestLogOut();
}
</script>

<style>
.header-strip {
    display:    grid;
    padding:    24px;
    place-self: start;
}

.badge-section {
    display:   flex;
    flex-flow: row nowrap;
    gap:       12px;
}

.login-appear-enter-active {
    transition: transform 100ms ease;
}

.login-appear-enter-from,
.login-appear-leave-to {
    transform: scale(0.9);
}
</style>