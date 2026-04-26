<template>
    <header class="navigation-bar">
        <div class="account">
            <AccountWidget />
        </div>

        <div class="navigation">
            <NavigationBarTabs v-model="selectedTab" class="navigation-bar-tabs" />
        </div>
    </header>
</template>

<script setup>
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navigation-bar {
    padding: 0 3px;

    align-items:           center;
    display:               grid;
    grid-template-columns: [links account] 1fr;
    grid-template-rows:    [links account] 80px;

    margin-bottom: 16px;
}

.account {
    grid-area: account;
}

.navigation {
    grid-area:    links;
    place-self: center;
}

/*
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
} */
</style>