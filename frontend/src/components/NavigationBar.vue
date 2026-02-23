<template>
    <header class="navbar">
        <AccountWidget class="account" />
        <NavigationBarTabs v-model="selectedTab" class="links" />
    </header>
</template>

<script setup>
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navbar {
    align-items:           center;
    display:               grid;
    grid-template-columns: [links account] 1fr;
    grid-template-rows:    [links account] 96px;
    padding:               0 24px;
}

.account {
    align-self: center;
    grid-area:  account;
}

.links {
    align-self:   center;
    grid-area:    links;
    justify-self: center;
}

@media (760px <= width < 1200px) {
    .navbar {
        grid-template-columns: [account] 1fr [links] auto;
    }
}

@media (width < 760px) {
    .navbar {
        grid-template-rows: [account] 64px 32px [links] 48px 8px;
        padding: 0 var(--edge-margin-x);
    }
}
</style>