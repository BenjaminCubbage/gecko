<template>
    <div class="account-widget">
        <transition name="login-appear" mode="out-in">
            <div 
                v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
                class="login-button-wrapper">
                <AccountWidgetLogInButton />
            </div>

            <div v-else-if="session.state.value === 'ready'" class="badge">
                <AccountWidgetProfileButton /> 
                <AccountWidgetUsernameBadge />
            </div>
        </transition>
    </div>
</template>

<script setup>
import { inject } from 'vue';

import AccountWidgetLogInButton   from './AccountWidgetLogInButton.vue';
import AccountWidgetProfileButton from './AccountWidgetProfileButton.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

import { Keys } from '@/core/di/keys.js';
const session = inject(Keys.SessionStore);
</script>

<style scoped>
.account-widget {
    display:      flex;
    height:       44px;
}

.badge {
    align-items: stretch;
    display:     flex;
    flex-flow:   row nowrap;
    gap:         4px;
}

.login-button-wrapper {
    display: grid;
}

.login-appear-enter-active,
.login-appear-leave-active {
    transition: transform 200ms ease, opacity 200ms ease;
}

.login-appear-enter-from,
.login-appear-leave-to {
    transform: translateY(-95px);
}
</style>