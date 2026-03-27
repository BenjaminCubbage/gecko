<template>
    <header class="navigation-bar">
        <AccountWidget class="account-widget" />
        <NavigationBarTabs v-model="selectedTab" class="navigation-bar-tabs" />
    </header>
</template>

<script setup>
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navigation-bar {
    align-items:           center;
    display:               grid;
    grid-template-columns: [links account] 1fr;
    grid-template-rows:    [links account] 82px;
    padding:               0 24px;
}

.account-widget {
    align-self: center;
    grid-area:  account;
}

.navigation-bar-tabs {
    align-self:   center;
    grid-area:    links;
    justify-self: center;
}

@media (760px <= width < 1200px) {
    .navigation-bar {
        grid-template-rows: [account] 96px 8px [links] 48px 8px;
    }
}

@media (width < 760px) {
    .navigation-bar {
        grid-template-rows: [account] 64px 32px [links] 48px 8px;
        padding: 0 var(--vp-margin);
    }
}
</style>