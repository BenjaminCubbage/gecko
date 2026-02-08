<template>
    <div class="header-strip">
        <transition name="login-appear" mode="out-in">
            <AccountWidgetLogInButton
                v-if="session.state.value !== 'ready'"
                href="/auth/login" />

            <div v-else>
                <div class="badge-section">
                    <AccountWidgetProfileButton @logout="logout" /> 

                    <AccountWidgetUsernameBadge
                        :username="session.activeUser.value['username']"
                        :status="userBadgeStatus"
                        :forbiddenUsernames="forbiddenUsernames"
                        @requestEdit="userBadgeStatus = 'editing'"
                        @cancel="userBadgeStatus = 'normal'"
                        @submit="changeUsername" />
                </div>
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
import AccountWidgetProfileButton from './AccountWidgetProfileButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

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
}

async function logout() {
    session.requestLogOut();
}
</script>

<style>
.header-strip {
    display:    grid;
    padding:    24px;
}

.badge-section {
    align-items: stretch;
    display:     flex;
    flex-flow:   row nowrap;
    gap:         4px;

    height: 43px;
}

.login-appear-enter-active {
    transition: transform 100ms ease;
}

.login-appear-enter-from,
.login-appear-leave-to {
    transform: scale(0.9);
}
</style>