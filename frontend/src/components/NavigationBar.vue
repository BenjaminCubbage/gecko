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
    display:     flex;
    flex-flow: row wrap;
    align-items: center;

    padding: 8px 0;
    gap:     8px;

    & > .widget { margin: 0 auto; flex: 1 0 0; min-width: max-content; }
    & > .tabs   { margin: auto; flex: 0 0 auto; }
    &::after    { flex: 1 1 0; content: ''; }

    @media (560px <= width < 930px) {
        grid-template: 
            "account . links" 70px /
             auto    1fr auto;
        padding: var(--vp-margin);
    }

    @media (width < 560px) {
        grid-template: 
            "account" auto
            "links"   70px /
             1fr;
        gap: 6px;
        padding: var(--vp-margin);
    }
}
</style>