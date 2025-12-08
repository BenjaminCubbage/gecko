<template>
    <div class="header-strip">
        <transition name="login-appear" mode="out-in">
            <LogInButton href="/auth/login" v-if="!session.loggedIn()" />
            
            <div v-else>
                <div class="badge-section">
                    <UserBadge
                        :username="session.activeUser().json()['username']"
                        :status="userBadgeStatus"
                        :forbiddenUsernames="forbiddenUsernames"
                        @requestEdit="userBadgeStatus = 'editing'"
                        @cancel="userBadgeStatus = 'normal'"
                        @submit="changeUsername" />

                    <LogOutButton v-show="userBadgeStatus == 'normal'" />
                </div>
                
                <StatusBubble ref="statusBubble" />
            </div>
        </transition>
    </div>
</template>

<script setup>
    import { inject, ref, useTemplateRef } from 'vue';
    import LogInButton from './LogInButton.vue';
    import LogOutButton from './LogOutButton.vue';
    import UserBadge from './UserBadge.vue';
    import StatusBubble from './StatusBubble.vue';
    import { Dispatch } from '@/core/dispatch/Dispatch.js';
    import { errorResponseToDisplayString } from '@/core/response_bodies/ErrorResponseToDisplayString';

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
            .onNetworkError(_ => statusBubble.value.showMessage('Couldn\'t connect to the server!'))
            .onHttpError(body => {
                statusBubble.value.showMessage(errorResponseToDisplayString(body))

                if (body?.error?.reason === 'username_taken')
                    forbiddenUsernames.value.push(newUsername);
            })
            .onError(_ => userBadgeStatus.value = 'editing');
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