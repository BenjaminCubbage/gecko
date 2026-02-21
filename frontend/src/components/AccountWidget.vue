<template>
    <div class="account-widget">
        <transition name="login-appear" mode="out-in">
            <div
                v-if="session.state.value === 'loggedout' || session.state.value === 'error'"
                class="login-button-wrapper">
                <AccountWidgetLogInButton />
            </div>

            <div
                v-else-if="session.state.value === 'ready'"
                class="widget-layout">
                <AccountWidgetDrawerToggle
                    ref="toggleEl"
                    v-model="isExpanded"
                    class="profile-button" />

                <transition name="drawer-open">
                    <div
                        v-show="isExpanded"
                        ref="drawerEl"
                        class="drawer">
                        <AccountWidgetLogOutButton style="position: absolute" />
                    </div>
                </transition>

                <AccountWidgetUsernameBadge class="username-badge" />
            </div>
        </transition>
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import AccountWidgetLogInButton   from './AccountWidgetLogInButton.vue';
import AccountWidgetLogOutButton  from './AccountWidgetLogOutButton.vue';
import AccountWidgetDrawerToggle  from './AccountWidgetDrawerToggle.vue';
import AccountWidgetUsernameBadge from './AccountWidgetUsernameBadge.vue';

import { useIsFocusWithin } from '@/composables/useIsFocusWithin';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

const isExpanded = ref(false);

const toggleEl = useTemplateRef('toggleEl');
const drawerEl = useTemplateRef('drawerEl');

const {
    isFocusWithin: isFocusWithinDrawer
} = useIsFocusWithin([ () => toggleEl.value?.innerElement, drawerEl ]);

watch(isFocusWithinDrawer, newValue => {
    if (!newValue)
        isExpanded.value = false;
});
</script>

<style scoped>
.account-widget {
    display: flex;
}

.widget-layout {
    align-items: stretch;
    display:     grid;
    flex-flow:   row nowrap;
    gap:         4px 7px;

    grid-template-areas:
        "profile username"
        "profile drawer";
}

.profile-button {
    grid-area: profile;
}

.username-badge {
    grid-area: username;
}

.login-button-wrapper {
    display: grid;
}

.drawer {
    position: relative;
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
    transform-origin: 0 0;
    transition: 
        scale     100ms ease,
        translate 100ms ease;
}

.drawer-open-enter-from,
.drawer-open-leave-to {
    scale:     0;
}
</style>