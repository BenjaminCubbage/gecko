<template>
    <nav class="navbar">
        <AccountWidget class="account" />
        <NavigationBarTabs v-model="selectedTab" class="pills" />
    </nav>
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
    grid-template-columns: [pills account] 1fr;
    grid-template-rows:    [pills account] 96px;
    padding:               0 24px;
}

.account {
    align-self: center;
    grid-area:  account;
}

.pills {
    align-self:   center;
    grid-area:    pills;
    justify-self: center;
}

@media (760px <= width < 1200px) {
    .navbar {
        grid-template-columns: [account] 1fr [pills] auto;
    }
}

@media (width < 760px) {
    .navbar {
        grid-template-rows: [account] 96px [pills] 48px;
        padding-bottom: 8px;
    }
}
</style>