<template>
    <header class="navigation-bar">
        <AccountWidget     class="widget" />
        <NavigationBarTabs class="tabs" v-model="selectedTab" />
    </header>
</template>

<script setup>
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navigation-bar {
    display: grid;

    padding: 8px var(--vp-margin);
    gap:     8px;

    grid-template: 
        "widget tabs ." auto /
         1fr    auto 1fr;

    & > .widget { grid-area: widget; }
    & > .tabs   { grid-area: tabs; }

    @media (width < 920px) {
        grid-template: 
            "widget tabs" auto /
            1fr    auto;
    }

    @media (width < 720px) {
        grid-template: 
            "widget" auto 
            "tabs"   auto /
             1fr;
    }
}
</style>