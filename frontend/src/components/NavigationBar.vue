<template>
    <header class="navigation-bar">
        <AccountWidget     class="widget" />
        <NavigationBarTabs
            v-if="session.state.value === 'ready'"
            class="tabs"
            v-model="selectedTab" />
    </header>
</template>

<script setup>
import { inject }        from 'vue';
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';
import { Keys }          from '@/core/di/keys.js';

const selectedTab = defineModel('selectedTab');
const session     = inject(Keys.SessionStore);
</script>

<style scoped>
.navigation-bar {
    display: grid;

    padding: 8px var(--vp-margin);
    gap:     8px;

    align-items: center;

    grid-template:
        "widget tabs ." 60px /
         1fr    auto 1fr;

    > .widget { grid-area: widget; }
    > .tabs   { grid-area: tabs; }

    @media (width < 920px) {
        grid-template:
            "widget tabs" 60px /
            1fr    auto;
    }

    @media (width < 720px) {
        grid-template:
            "widget" 45px
            "tabs"   auto /
             1fr;
    }
}
</style>