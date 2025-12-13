<template>
    <div class="header-strip">
        <transition name="login-appear" mode="out-in">
            <AccountWidgetLogInButton 
                v-if="!session.loggedIn()" 
                href="/auth/login" />
            
            <div v-else>
                <div class="badge-section">
                    <AccountWidgetUsernameBadge
                        :username="session.activeUser().json()['username']"
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
import { inject, ref, useTemplateRef } from 'vue';
import AccountWidgetLogInButton from './AccountWidgetLogInButton.vue';
import AccountWidgetLogOutButton from './AccountWidgetLogOutButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';
import AccountWidgetStatusBubble from './AccountWidgetStatusBubble.vue';

import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { errorResponseToDisplayString } from '@/core/http/ErrorResponseToDisplayString';

const statusBubble = useTemplateRef('statusBubble');
const session = inject('session');

const userBadgeStatus = ref('normal');
const forbiddenUsernames = ref([]);

function changeUsername(newUsername) {
    userBadgeStatus.value = 'pending';

    Dispatch.Patch_ChangeUsername(session.value.activeUser().json()['user_id'], newUsername)
        .onSuccess(() => {
            session.value.activeUser().json()['username'] = newUsername;
            userBadgeStatus.value = 'normal';
        })
        .onNetworkError(() => statusBubble.value.showMessage('Couldn\'t connect to the server!'))
        .onHttpError(body => {
            statusBubble.value.showMessage(errorResponseToDisplayString(body))

            if (body?.error?.reason === 'username_taken')
                forbiddenUsernames.value.push(newUsername);
        })
        .onError(() => userBadgeStatus.value = 'editing');
}

function logout() {
    Dispatch.Post_LogOut(session.value)
        .onSuccess(() => session.value.setActiveUser(null))
        .onHttpError((body, status) => console.warn(`Couldn't POST_LogOut. code: ${status} body: ${body}`));
}
</script>

<style>
    .header-strip {
        place-self: start;

        display: grid;
        place-items: start;
        place-content: start;

        padding: 24px;
    }

    .badge-section {
        display: flex;
        flex-flow: row nowrap;
        gap: 12px;
    }

    .login-appear-enter-active {
        transition: transform 100ms ease;
    }

    .login-appear-enter-from,
    .login-appear-leave-to {
        transform: scale(0.9);
    }
</style>