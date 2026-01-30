<template>
    <nav class="navbar">
        <AccountWidget class="account" />
        <NavbarTabs v-model="selectedTab" class="pill" />
    </nav>
</template>

<script setup>
import AccountWidget from '@/components/account_widget/AccountWidget.vue';
import NavbarTabs    from './NavbarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navbar {
    align-items: center;
    display:     grid;
    height:      96px;
}

.account,
.pill {
    grid-area: 1 / 1;
}

.pill {
    align-self:   center;
    justify-self: center;
}
</style>